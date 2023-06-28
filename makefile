CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: main.cpp VulkanRenderer.o VulkanRenderer.h Utils.h
	g++ $(CFLAGS) -o VulkanTest VulkanRenderer.o main.cpp $(LDFLAGS) 

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest