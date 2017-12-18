
#https://gomputor.wordpress.com/2008/09/22/convert-a-file-in-utf-8-or-any-encoding-with-python/
#!/usr/bin/env python
 
import os
import codecs
import sys
import chardet

def convert_file(filePath):
    if any([filePath.endswith(extension) for extension in '.h,.cpp,.hxx,.rc,.inl'.split(',')]):
        try:

            with codecs.open(filePath, "rb", "ansi") as SrcFile:

                print(u"%s converting...." % filePath)

                text = SrcFile.read()

                try:
                    with codecs.open(filePath, "wb", "utf-8") as DestFile:
                        DestFile.write(text)
                        print(u"%s converted." % filePath)
                except:
                    print(u"ERROR in converting.")
        except:
            print(u"ERROR in open.")

        print ('-'*40)

if __name__ == '__main__':
    if len(sys.argv[1:]) == 0:
        try:
            path = raw_input(u"Input pah of file:")
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
                if path.endswith('/'):
                    filePath = path + file
                else:
                    filePath = path + '/' + file
               # filePath = filePath.decode("utf-8")
                convert_file(filePath)
    else:
        if os.path.isfile(path) :
            convert_file(path)
sys.exit(0)
