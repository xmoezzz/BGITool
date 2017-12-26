# BGI Tool
Tools work with BGI VN engine.  


### BGIDev
BGIUpkV1 works with some old versions while BGIUpkV2 works with newer versions.  
It can only unpack archives.  

### BGIDecompress 
BGIDecompress works with old version texture, it can convert BGI texture into bitmap image.

### BGINewDecompress 
BGIDecompress works with newer version texture, it can convert BGI texture into bitmap image.

notice: The texture's structure and crypto may keep changing.

### BGITextureConverter
Convert bitmap image (*.bmp) into uncompressed BGI texture.

### BGIDisasm
Disassemble BGI compiled script.

### BGITextDumper
Dump text from BGI compiled script(bytecode reference order).

### BGITextDumperBinaryOrder
Dump text from BGI compiled script(string pool order).

### DscDecoder
*.dsc file decode.

## How to repack?
1.use molebox.(recommend)  
2.repack to original archive.(make tool by yourself)  
3.write a custom patch.(see _patchSample)  

### Why molebox
1.Very easy.
2.just pull game executable file and translated files into the same directory them pack them by using melobox.  


## Compile 
Require VS2013

