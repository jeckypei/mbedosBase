find . -name "*.h" -o -name "*.c" -o -name "*.cpp" -o -name "*.java" -o -name "*.s" -o -name "*.asm" -o -name "*.cc" \
        >cscope.files  
cscope -bkq -i cscope.files  
ctags -R  
