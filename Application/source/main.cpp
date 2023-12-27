#include "Application.h"


int main() {
	using namespace Engine;

	Application engineApp;

	if (engineApp.Initialize()) {
		while (engineApp.IsRunning()) {
			engineApp.Update();
		}
	}
	

}