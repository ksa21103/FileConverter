
#https://gomputor.wordpress.com/2008/09/22/convert-a-file-in-utf-8-or-any-encoding-with-python/
#!/usr/bin/env python
 
import os
import sys
import chardet

def convert_file(filePath):
    if any([filePath.endswith(extension) for extension in '.h,.cpp,.hxx,.rc,.inl'.split(',')]):
        with open(filePath, "rb") as SrcFile:
            text = SrcFile.read()
            enc = chardet.detect(text).get("encoding")
            if enc and enc.lower() != "utf-8":
                try:
                    text = text.decode(enc)
                    text = text.encode("utf-8")
                    with open(filePath, "wb") as DestFile:
                        DestFile.write(text)
                        print(u"%s converted." % filePath)
                except:
                    print (u"Error in file name: filename contains russian chars.")
            else:
                print (u"File %s was in dest %s encoding" % (filePath, enc))
            print ('-'*40)

if __name__ == '__main__':
    if len(sys.argv[1:]) == 0:
        try:
            path = raw_input(u"Input payh of file:")
        except KeyboardInterrupt:
            print( u"Input cancelled.")
            sys.exit(0)
    else:
        path = sys.argv[1:][0]
    if os.path.isdir(path):
        last=path[-1]
        if last.endswith('/'):
            path = path
        else:
            path = path + "/"
        print (path)
        for (path, dirs, files) in os.walk(path):
            for file in files:
                filePath = path + file
               # filePath = filePath.decode("utf-8")
                convert_file(filePath)
    else:
        if os.path.isfile(path) :
            convert_file(path)
sys.exit(0)
