using namespace std;

#include <Classes/SceneManager.h>

int main() {
	try {
		SceneManager *scene = new SceneManager;

		scene -> Initialize(802, 462);

		scene -> Run();

		scene -> Finish();

	} catch (const exception &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
	
}