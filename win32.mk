src += $(wildcard src/win32/*.c)

I += 

l += -Llib  -lgdi32 

flags += 


include base.mk
