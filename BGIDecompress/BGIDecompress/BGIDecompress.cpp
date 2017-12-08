#include <Windows.h>
#include <cstdio>
#include <string>

using std::string;
using std::wstring;

typedef char s8;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;


#pragma pack (1)
typedef struct
{
	s8 magic[16];			/* "CompressedBG___" */
	u16 width;
	u16 height;
	u32 color_depth;
	u32 reserved0[2];
	u32 zero_comprlen;		/* huffman解压后的0值字节压缩的数据长度 */
	u32 key;				/* 解密叶节点权值段用key */
	u32 encode_length;		/* 加密了的叶节点权值段的长度 */
	u8 sum_check;
	u8 xor_check;
	u16 reserved1;
} bg_header_t, BGHeader;
#pragma pack ()

struct bits
{
	unsigned long curbits;
	unsigned long curbyte;
	unsigned char cache;
	unsigned char *stream;
	unsigned long stream_length;
};

void bits_init(struct bits *bits, unsigned char *stream, unsigned long stream_length)
{
	memset(bits, 0, sizeof(*bits));
	bits->stream = stream;
	bits->stream_length = stream_length;
}

#if 1

#if 1
int bits_get_high(struct bits *bits, unsigned int req_bits, unsigned int *retval)
{
	unsigned int bits_value = 0;

	while (req_bits > 0) {
		unsigned int req;

		if (!bits->curbits) {
			if (bits->curbyte >= bits->stream_length)
				return -1;
			bits->cache = bits->stream[bits->curbyte++];
			bits->curbits = 8;
		}

		if (bits->curbits < req_bits)
			req = bits->curbits;
		else
			req = req_bits;

		bits_value <<= req;
		bits_value |= bits->cache >> (bits->curbits - req);
		bits->cache &= (1 << (bits->curbits - req)) - 1;
		req_bits -= req;
		bits->curbits -= req;
	}
	*retval = bits_value;
	return 0;
}
#else
static BYTE table0[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static BYTE table1[8] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

int bits_get_high(struct bits *bits, unsigned int req_bits, unsigned int *retval)
{
	unsigned int bits_value = 0;

	for (int i = req_bits - 1; i >= 0; i--) {
		if (!bits->curbits) {
			if (bits->curbyte >= bits->stream_length)
				return -1;

			bits->cache = bits->stream[bits->curbyte++];
		}

		if (bits->cache & table0[bits->curbits++])
			bits_value |= table1[i & 7];

		bits->curbits &= 7;
		if (!(i & 7) && i)
			bits_value <<= 8;
	}
	*retval = bits_value;

	return 0;
}
#endif

int bit_get_high(struct bits *bits, void *retval)
{
	return bits_get_high(bits, 1, (unsigned int *)retval);
}

#else

/* 把距离最高位最近的地方的值返回给retval */
int bit_get_high(struct bits *bits, void *retval)
{
	if (!bits->curbits) {
		if (bits->curbyte >= bits->stream_length)
			return -1;

		bits->cache = bits->stream[bits->curbyte++];
		bits->curbits = 8;
	}
	bits->curbits--;
	*(unsigned char *)retval = bits->cache >> bits->curbits;
	bits->cache &= (1 << bits->curbits) - 1;
	return 0;
}

/* FIXME：实现错误（应该是把距离最高位最近的地方的值依次返回给retval的从高到低字节） */
#if 1
int bits_get_high(struct bits *bits, unsigned int req_bits, unsigned int *retval)
{
	for (unsigned int i = 0; i < req_bits; i++) {
		unsigned char bitval;

		if (bit_get_high(bits, &bitval))
			return -1;

		*retval <<= 1;
		*retval |= bitval & 1;
	}
	*retval &= (1 << req_bits) - 1;

	return 0;
}
#else
int bits_get_high(struct bits *bits, unsigned int req_bits, unsigned int *retval)
{
	unsigned char byteval = 0;

	for (unsigned int i = 0; i < req_bits; i++) {
		unsigned char bitval;

		if (bit_get_high(bits, &bitval))
			return -1;

		byteval <<= 1;
		byteval |= bitval & 1;

		if (!((i + 1) & 7)) {
			((unsigned char *)retval)[i / 8] = byteval;
			byteval = 0;
		}
	}
	if (i & 7)
		((unsigned char *)retval)[(i - 1) / 8] = byteval;

	return 0;
}
#endif

#endif

/* 把setval的最低位设置到离最高位最近的地方开始 */
int bit_put_high(struct bits *bits, unsigned char setval)
{
	bits->curbits++;
	bits->cache |= (setval & 1) << (8 - bits->curbits);
	if (bits->curbits == 8) {
		if (bits->curbyte >= bits->stream_length)
			return -1;
		bits->stream[bits->curbyte++] = bits->cache;
		bits->curbits = 0;
		bits->cache = 0;
	}
	return 0;
}

/* 按照从高字节到低字节的顺序把setval中的值设置到离最高位最近的地方开始 */
int bits_put_high(struct bits *bits, unsigned int req_bits, void *setval)
{
	unsigned int this_bits;
	unsigned int this_byte;
	unsigned int i;

	this_byte = req_bits / 8;
	this_bits = req_bits & 7;
	for (int k = (int)this_bits - 1; k >= 0; k--) {
		unsigned char bitval;

		bitval = !!(((unsigned char *)setval)[this_byte] & (1 << k));
		if (bit_put_high(bits, bitval))
			return -1;
	}
	this_bits = req_bits & ~7;
	this_byte--;
	for (i = 0; i < this_bits; i++) {
		unsigned char bitval;

		bitval = !!(((unsigned char *)setval)[this_byte - i / 8] & (1 << (7 - (i & 7))));
		if (bit_put_high(bits, bitval))
			return -1;
	}

	return 0;
}

void bits_flush(struct bits *bits)
{
	bits->stream[bits->curbyte] = bits->cache;
}

static u8 update_key(u32 *key, u8 *magic)
{
	u32 v0, v1;

	v0 = (*key & 0xffff) * 20021;
	v1 = (magic[1] << 24) | (magic[0] << 16) | (*key >> 16);
	v1 = v1 * 20021 + *key * 346;
	v1 = (v1 + (v0 >> 16)) & 0xffff;
	*key = (v1 << 16) + (v0 & 0xffff) + 1;
	return v1 & 0x7fff;
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

static unsigned int	bg_create_huffman_tree(bg_huffman_node *nodes, u32 *leaf_nodes_weight)
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

static unsigned int zero_decompress(unsigned char *uncompr, unsigned int uncomprlen,
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

static void bg_average_defilting(unsigned char *dib_buf, unsigned int width,
	unsigned int height, unsigned int bpp)
{
	unsigned int line_len = width * bpp;

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			for (unsigned int p = 0; p < bpp; p++) {
				unsigned int a, b;
				unsigned int avg;

				b = y ? dib_buf[(y - 1) * line_len + x * bpp + p] : -1;
				a = x ? dib_buf[y * line_len + (x - 1) * bpp + p] : -1;
				avg = 0;

				if (a != -1)
					avg += a;
				if (b != -1)
					avg += b;
				if (a != -1 && b != -1)
					avg /= 2;

				dib_buf[y * line_len + x * bpp + p] += avg;
			}
		}
	}
}


static void	decode_bg(unsigned char *enc_buf, unsigned int enc_buf_len,
	unsigned long key, unsigned char *ret_sum, unsigned char *ret_xor)
{
	unsigned char sum = 0;
	unsigned char _xor = 0;

	unsigned long a = 0, b = 0, c = 0;
	for (unsigned long i = 0; i < enc_buf_len; i++)
	{
		a = (key & 0xffff) * 20021;
		b = ((key >> 16) & 0xffff) * 20021;
		c = key * 346 + b; c += (a >> 16) & 0xffff;
		key = (c << 16) + (a & 0xffff) + 1;
		//key每次都会被更新
		(*enc_buf) -= (char)c;      //原始数据 - 结果低8位
		enc_buf++;
	}

	*ret_sum = sum;
	*ret_xor = _xor;
}

/*
static void	decode_bg(unsigned char *enc_buf, unsigned int enc_buf_len,
	unsigned long key, unsigned char *ret_sum, unsigned char *ret_xor)
{
	unsigned char sum = 0;
	unsigned char _xor = 0;
	u8 magic[2] = { 0, 0 };

	for (unsigned int i = 0; i < enc_buf_len; i++) {
		enc_buf[i] -= update_key(&key, magic);
		sum += enc_buf[i];
		_xor ^= enc_buf[i];
	}
	*ret_sum = sum;
	*ret_xor = _xor;
}
*/

int DCEncode(int a1)
{
	unsigned __int16 v1; // ax@1

	v1 = (20021 * (unsigned int)(unsigned __int16)*(DWORD *)a1 >> 16)
		+ 346 * *(DWORD *)a1
		+ 20021 * (*(DWORD *)a1 >> 16);
	*(DWORD *)a1 = (v1 << 16) + (unsigned __int16)(20021 * *(DWORD *)a1) + 1;
	return v1 & 0x7FFF;
}

void DoDCEncode(int a1, unsigned int a2, char a3)
{
	unsigned int v3; // edi@1

	v3 = 0;
	if (a2)
	{
		do
		{
			*(BYTE *)(v3++ + a1) -= DCEncode((int)&a3);
		}
		while (v3 < a2);
	}
}

static unsigned int bg_decompress(bg_header_t *bg_header, unsigned int bg_len,
	unsigned char *image_buf, unsigned int image_size)
{
	unsigned int act_uncomprlen = 0;
	unsigned int i;
	unsigned char *enc_buf = (unsigned char *)(bg_header + 1);

	/* 解密叶节点权值 */
	unsigned char sum;
	unsigned char _xor;
	decode_bg(enc_buf, bg_header->encode_length, bg_header->key, &sum, &_xor);

	/*
	if (sum != bg_header->sum_check || _xor != bg_header->xor_check)
		return 0;
	*/

	/* 初始化叶节点权值 */
	u32 leaf_nodes_weight[256];
	unsigned int curbyte = 0;
	for (i = 0; i < 256; i++) {
		unsigned int bits = 0;
		u32 weight = 0;
		unsigned char code;

		do {
			if (curbyte >= bg_header->encode_length)
				return 0;
			code = enc_buf[curbyte++];
			weight |= (code & 0x7f) << bits;
			bits += 7;
		} while (code & 0x80);
		leaf_nodes_weight[i] = weight;
	}

	bg_huffman_node nodes[511];
	unsigned int root_node_index = bg_create_huffman_tree(nodes, leaf_nodes_weight);
	unsigned char *zero_compr = (unsigned char *)malloc(bg_header->zero_comprlen);
	if (!zero_compr)
		return 0;

	unsigned char *compr = enc_buf + bg_header->encode_length;
	unsigned int comprlen = bg_len - sizeof(bg_header_t)-bg_header->encode_length;
	act_uncomprlen = bg_huffman_decompress(nodes, root_node_index,
		zero_compr, bg_header->zero_comprlen, compr, comprlen);
	if (act_uncomprlen != bg_header->zero_comprlen) {
		free(zero_compr);
		return 0;
	}

	act_uncomprlen = zero_decompress(image_buf, image_size, zero_compr, bg_header->zero_comprlen);
	free(zero_compr);

	bg_average_defilting(image_buf, bg_header->width, bg_header->height, bg_header->color_depth / 8);

	return act_uncomprlen;
}


int write_bmp(const wchar_t* filename,
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

FORCEINLINE int wmain(int argc, wchar_t** argv)
{
	if (argc != 2)
	{
		MessageBoxW(NULL, L"BGI Decompression\n<xmoe.project@gmail.com>", L"BGI Tool", MB_OK);
		exit(-1);
	}
	FILE *fin = _wfopen(argv[1], L"rb");
	if (fin == NULL)
	{
		fclose(fin);
		return -1;
	}
	fseek(fin, 0, SEEK_END);
	DWORD FileSize = ftell(fin);
	rewind(fin);
	char *pFile = new char[FileSize];
	fread(pFile, FileSize, 1, fin);
	fclose(fin);

	BGHeader *bg_header;
	bg_header = (BGHeader*)pFile;

	if (!memcmp(bg_header->magic, "CompressedBG___", 16) /* && bg_header->reserved1 == 1*/)
	{
		unsigned int image_size;
		unsigned char *image_buffer;
		image_size = bg_header->width * bg_header->height * bg_header->color_depth / 8;
		image_buffer = new unsigned char[image_size];

		unsigned long act_uncomprlen = bg_decompress(bg_header, FileSize,
			image_buffer, image_size);
		if (act_uncomprlen != image_size)
		{
			WCHAR Info[256] = { 0 };
			wsprintfW(Info, L"Unequal size [Cal:%08x] [Decode:%08x]", image_size, act_uncomprlen);
			MessageBoxW(NULL, Info, L"BGIDecompress", MB_OK);
			delete[] image_buffer;
			return -1;
		}
		wchar_t szFileName[MAX_PATH] = {0};
		wsprintfW(szFileName, L"%s.bmp", argv[1]);
		//we should flip it
		write_bmp(szFileName, image_buffer, image_size, bg_header->width, -bg_header->height, bg_header->color_depth / 8);
		delete[] image_buffer;
	}
	else
	{
		MessageBoxW(NULL, L"Unsupported BG Format", L"BGIDecompress", MB_OK);
	}
	delete[] pFile;
	return 0;
}
