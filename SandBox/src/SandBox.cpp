#include <Engine.h>  
#include <iostream> // Ensure this is included for std::cout  

class SandBox : public Engine::Application  
{  
public:  
SandBox() {  

}  

~SandBox() {  
}  

};  

Engine::Application* Engine::createApplication() {  
return new SandBox();  
}
