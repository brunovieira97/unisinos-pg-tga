using namespace std;

#include <Classes/SceneManager.h>

int main() {
	SceneManager *scene = new SceneManager;

	scene -> initialize(1000, 800);

	scene -> run();

	scene -> finish();

	return 0;
}