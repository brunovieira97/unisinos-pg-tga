using namespace std;

#include <Classes/SceneManager.h>

int main() {
	SceneManager *scene = new SceneManager;

	scene -> Initialize(802, 462);

	scene -> Run();

	scene -> Finish();

	return 0;
}