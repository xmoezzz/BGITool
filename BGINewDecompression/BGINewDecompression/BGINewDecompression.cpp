#include "stdafx.h"
#include <Windows.h>
#include "WinFile.h"
#include "BitsInfo.h"
#include "BGHeader.h"

typedef struct CompressedBGHaeder
{
	BYTE Magic[16];
	WORD Width;
	WORD Height;
	DWORD ColorDepth;
	DWORD Unk1;
	DWORD Unk2;
	DWORD ZeroComprlen;
	DWORD Key;
	DWORD EncodeLength;
	BYTE SumCheck;
	BYTE XorCheck;
	WORD InfoEx;
}_CompressedBGHeader, *pCompressedBGHeader;

PVOID WINAPI MemoryAlloc(ULONG Size)
{
	return HeapAlloc(GetProcessHeap(), 0, Size);
}

VOID WINAPI MemoryFree(PVOID pBuffer)
{
	HeapFree(GetProcessHeap(), 0, pBuffer);
}

unsigned char UpdateKeyInner(unsigned long *key, unsigned char *magic)
{
	unsigned long v0, v1;

	v0 = (*key & 0xffff) * 20021;
	v1 = (magic[1] << 24) | (magic[0] << 16) | (*key >> 16);
	v1 = v1 * 20021 + *key * 346;
	v1 = (v1 + (v0 >> 16)) & 0xffff;
	*key = (v1 << 16) + (v0 & 0xffff) + 1;
	return v1 & 0x7fff;
}


void UpdateKey(unsigned char *enc_buf, unsigned int enc_buf_len, unsigned long key)
{
	unsigned char sum = 0;
	unsigned char _xor = 0;
	unsigned char magic[2] = { 0, 0 };

	for (unsigned int i = 0; i < enc_buf_len; i++) 
	{
		enc_buf[i] -= UpdateKeyInner(&key, magic);
		sum += enc_buf[i];
		_xor ^= enc_buf[i];
	}
}


BOOL DecryptionData(char* a1, signed int a2, char a3, char a4)
{
	char v4; // al@1
	signed int v5; // edi@1
	char v6; // bl@1
	unsigned int v7; // ecx@1
	char v8; // al@2
	char v9; // dl@2
	char v11; // [sp+Ah] [bp-2h]@1
	char v12; // [sp+Bh] [bp-1h]@1

	v4 = 0;
	v5 = a2;
	v6 = 0;
	v7 = 0;
	v11 = 0;
	v12 = 0;
	if (a2 >= 2)
	{
		do
		{
			v8 = *(v7 + a1 + 1);
			v9 = *(v7 + a1);
			v11 += v9;
			v6 += v8;
			v12 ^= v9 ^ v8;
			v7 += 2;
		} while (v7 < a2 - 1);
		v4 = 0;
		v5 = a2;
	}
	if (v7 < v5)
	{
		v4 = *(v7 + a1);
		v12 ^= v4;
	}
	return a3 == v4 + v11 + v6 && a4 == v12;
}

int write_bmp(const WCHAR* filename,
	unsigned char*   buff,
	unsigned long    len,
	unsigned long    width,
	unsigned long    height,
	unsigned short   depth_bytes)
{
	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;

	memset(&bmf, 0, sizeof(bmf));
	memset(&bmi, 0, sizeof(bmi));

	bmf.bfType = 0x4D42;
	bmf.bfSize = sizeof(bmf)+sizeof(bmi)+len;
	bmf.bfOffBits = sizeof(bmf)+sizeof(bmi);

	bmi.biSize = sizeof(bmi);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = depth_bytes * 8;

	FILE* fd = _wfopen(filename, L"wb");
	fwrite(&bmf, 1, sizeof(bmf), fd);
	fwrite(&bmi, 1, sizeof(bmi), fd);
	fwrite(buff, 1, len, fd);
	fclose(fd);
	return 0;
}

typedef struct {
	unsigned int valid;				/* 是否有效的标记 */
	unsigned int weight;			/* 权值 */
	unsigned int is_parent;			/* 是否是父节点 */
	unsigned int parent_index;		/* 父节点索引 */
	unsigned int left_child_index;	/* 左子节点索引 */
	unsigned int right_child_index;	/* 右子节点索引 */
} bg_huffman_node;

static unsigned int bg_huffman_decompress(bg_huffman_node *huffman_nodes,
	unsigned int root_node_index,
	unsigned char *uncompr, unsigned int uncomprlen,
	unsigned char *compr, unsigned int comprlen)
{
	struct bits bits;

	bits_init(&bits, compr, comprlen);
	unsigned int act_uncomprlen;
	for (act_uncomprlen = 0; act_uncomprlen < uncomprlen; act_uncomprlen++) {
		unsigned char child;
		unsigned int node_index;

		node_index = root_node_index;
		do {
			if (bit_get_high(&bits, &child))
				goto out;

			if (!child)
				node_index = huffman_nodes[node_index].left_child_index;
			else
				node_index = huffman_nodes[node_index].right_child_index;
		} while (huffman_nodes[node_index].is_parent);

		uncompr[act_uncomprlen] = node_index;
	}
out:
	return act_uncomprlen;
}

static unsigned int	bg_create_huffman_tree(bg_huffman_node *nodes, ULONG *leaf_nodes_weight)
{
	unsigned int parent_node_index = 256;	/* 父节点从nodes[]的256处开始 */
	bg_huffman_node *parent_node = &nodes[parent_node_index];
	unsigned int root_node_weight = 0;	/* 根节点权值 */
	unsigned int i;

	/* 初始化叶节点 */
	for (i = 0; i < 256; i++) {
		nodes[i].valid = !!leaf_nodes_weight[i];
		nodes[i].weight = leaf_nodes_weight[i];
		nodes[i].is_parent = 0;
		root_node_weight += nodes[i].weight;
	}

	while (1) {
		unsigned int child_node_index[2];

		/* 创建左右子节点 */
		for (i = 0; i < 2; i++) {
			unsigned int min_weight;

			min_weight = -1;
			child_node_index[i] = -1;
			/* 遍历nodes[], 找到weight最小的2个节点作为子节点 */
			for (unsigned int n = 0; n < parent_node_index; n++) {
				if (nodes[n].valid) {
					if (nodes[n].weight < min_weight) {
						min_weight = nodes[n].weight;
						child_node_index[i] = n;
					}
				}
			}
			/* 被找到的子节点标记为无效，以便不参与接下来的查找 */
			nodes[child_node_index[i]].valid = 0;
			nodes[child_node_index[i]].parent_index = parent_node_index;
		}
		/* 创建父节点 */
		parent_node->valid = 1;
		parent_node->is_parent = 1;
		parent_node->left_child_index = child_node_index[0];
		parent_node->right_child_index = child_node_index[1];
		parent_node->weight = nodes[parent_node->left_child_index].weight
			+ nodes[parent_node->right_child_index].weight;
		if (parent_node->weight == root_node_weight)
			break;
		parent_node = &nodes[++parent_node_index];
	}

	return parent_node_index;
}

static unsigned char update_key(unsigned long *key, unsigned char *magic)
{
	unsigned long v0, v1;

	v0 = (*key & 0xffff) * 20021;
	v1 = (magic[1] << 24) | (magic[0] << 16) | (*key >> 16);
	v1 = v1 * 20021 + *key * 346;
	v1 = (v1 + (v0 >> 16)) & 0xffff;
	*key = (v1 << 16) + (v0 & 0xffff) + 1;
	return v1 & 0x7fff;
}


void decode_bg(unsigned char *enc_buf, unsigned int enc_buf_len,
	unsigned long key, unsigned char *ret_sum, unsigned char *ret_xor)
{
	unsigned char sum = 0;
	unsigned char _xor = 0;
	unsigned char magic[2] = { 0, 0 };

	for (unsigned int i = 0; i < enc_buf_len; i++)
	{
		enc_buf[i] -= update_key(&key, magic);
		sum += enc_buf[i];
		_xor ^= enc_buf[i];
	}
	*ret_sum = sum;
	*ret_xor = _xor;
}

unsigned int zero_decompress(unsigned char *uncompr, unsigned int uncomprlen,
	unsigned char *compr, unsigned int comprlen)
{
	unsigned int act_uncomprlen = 0;
	int dec_zero = 0;
	unsigned int curbyte = 0;

	while (1) {
		unsigned int bits = 0;
		unsigned int copy_bytes = 0;
		unsigned char code;

		do {
			if (curbyte >= comprlen)
				goto out;

			code = compr[curbyte++];
			copy_bytes |= (code & 0x7f) << bits;
			bits += 7;
		} while (code & 0x80);

		if (act_uncomprlen + copy_bytes > uncomprlen)
			break;
		if (!dec_zero && (curbyte + copy_bytes > comprlen))
			break;

		if (!dec_zero) {
			memcpy(&uncompr[act_uncomprlen], &compr[curbyte], copy_bytes);
			curbyte += copy_bytes;
			dec_zero = 1;
		}
		else {
			memset(&uncompr[act_uncomprlen], 0, copy_bytes);
			dec_zero = 0;
		}
		act_uncomprlen += copy_bytes;
	}
out:
	return act_uncomprlen;
}


unsigned int bg_decompress(CompressedBGHaeder *bg_header, unsigned int bg_len,
	unsigned char *image_buf, unsigned int image_size)
{
	unsigned int act_uncomprlen = 0;
	unsigned int i;
	unsigned char *enc_buf = (unsigned char *)(bg_header + 1);

	/* 解密叶节点权值 */
	unsigned char sum;
	unsigned char _xor;
	decode_bg(enc_buf, bg_header->EncodeLength, bg_header->Key, &sum, &_xor);
	if (sum != bg_header->SumCheck || _xor != bg_header->XorCheck)
	{
		MessageBoxW(NULL, L"CheckSum Error", L"Error", MB_OK);
		ExitProcess(-1);
	}

	/* 初始化叶节点权值 */
	ULONG leaf_nodes_weight[256];
	unsigned int curbyte = 0;
	for (i = 0; i < 256; i++)
	{
		unsigned int bits = 0;
		ULONG weight = 0;
		unsigned char code;

		do
		{
			/*
			if (curbyte > bg_header->EncodeLength)
			{
				printf("Info>> Size : %d, Cur : %d\n", bg_header->EncodeLength, curbyte);
				MessageBoxW(NULL, L"Failed to build nodes", L"Error", MB_OK);
				ExitProcess(-1);
			}
			*/
			
			code = enc_buf[curbyte++];
			weight |= (code & 0x7f) << bits;
			bits += 7;
		}
		while (code & 0x80);
		leaf_nodes_weight[i] = weight;
	}

	//printf("Size : %d, Cur : %d\n", bg_header->EncodeLength, curbyte);
	//getchar();


	bg_huffman_node nodes[511];
	unsigned int root_node_index = bg_create_huffman_tree(nodes, leaf_nodes_weight);
	unsigned char *zero_compr = (unsigned char *)MemoryAlloc(bg_header->ZeroComprlen);
	if (!zero_compr)
	{
		return 0;
	}

	unsigned char *compr = enc_buf + bg_header->EncodeLength;
	unsigned int comprlen = bg_len - sizeof(CompressedBGHaeder)-bg_header->EncodeLength;
	act_uncomprlen = bg_huffman_decompress(nodes, root_node_index,
		zero_compr, bg_header->ZeroComprlen, compr, comprlen);
	if (act_uncomprlen != bg_header->ZeroComprlen)
	{
		MessageBoxW(NULL, L"Failed to decode huffman tree", L"Error", MB_OK);
		MemoryFree(zero_compr);
		return 0;
	}
	else
	{
		printf("Ok, Huffman Decode Size: Raw->%d, Act->%d\n", bg_header->ZeroComprlen, act_uncomprlen);
		printf("Uncomp Len : %d, Zero CompressLength->%d\n", act_uncomprlen, bg_header->ZeroComprlen);
	}

	if (bg_header->ZeroComprlen)
	{
		act_uncomprlen = zero_decompress(image_buf, image_size, zero_compr, bg_header->ZeroComprlen);
	}
	else
	{
		write_bmp(L"Dump.bmp",
			(unsigned char*)(bg_header + sizeof(CompressedBGHaeder)),
			bg_len - sizeof(CompressedBGHaeder),
			bg_header->Width,
			bg_header->Width,
			bg_header->ColorDepth / 8);
	}
	MemoryFree(zero_compr);

	/*
	bg_average_defilting(image_buf, bg_header->width, bg_header->height, bg_header->color_depth / 8);
	*/
	return act_uncomprlen;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		return 0;
	}
	
	WinFile InputFile;
	if (InputFile.Open(argv[1], WinFile::FileRead) != S_OK)
	{
		MessageBoxW(NULL, L"Failed to open", L"Error", MB_OK);
		return 0;
	}
	ULONG InputSize = InputFile.GetSize32();
	PBYTE Buffer = (BYTE*)MemoryAlloc(InputSize);
	InputFile.Read(Buffer, InputSize);
	InputFile.Release();

	CBG pic;
	if (pic.Uncompress((char*)Buffer, InputSize))
	{
		WCHAR name[500] = { 0 };
		lstrcpyW(name, argv[1]);
		lstrcatW(name, L".bmp");
		WinFile OutFile;
		if (OutFile.Open(name, WinFile::FileWrite) == S_OK)
		{
			OutFile.Write((PBYTE)(pic.BmpData), pic.Size);
			printf("Ok\n");
		}
		else
		{ 
			printf("Failed\n");
		}
	}

	MemoryFree(Buffer);
	return 0;
}

