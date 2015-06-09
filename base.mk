src += $(wildcard src/*.c)
src += $(wildcard src/pack/*.c)

out = bin/dir_sync.exe

obj_dir = obj
obj = $(patsubst %.c, $(obj_dir)/%.o, $(src))

I += -Isrc -Ilib  
l += -Llib 

flags += -std=c99 


all : $(out) run

run : 
	@$(out) src to_src

$(out) : $(obj)
	@mkdir -p $(dir $(out))
	gcc -s -o $(out) $^ $(flags) $(l)


clean : 
	@rm -rf bin
	@rm -rf obj

sinclude $(obj:.o=.d)

$(obj_dir)/%.o : %.c
	@mkdir -p $(dir $@)
	@gcc -MM -MP -MT $@ -MF $(patsubst %.o, %.d, $@) $< $(I) $(flags)
	@echo 'compile $< => $@'
	@gcc -o $@ -c $< $(I) $(flags)
