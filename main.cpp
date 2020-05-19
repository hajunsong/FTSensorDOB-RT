#include <QCoreApplication>
#include "FTSensor/ftsensor.h"
#include "KeyInput/keyinput.h"

static QCoreApplication *app;
static KeyInput *keyInput;
static FTSensor *ftsensor;

void keyInputSlot(char key){
    if (key == 'q' || key == 27){
        printf("Pressed 'q' or 'ESC'\n");
        delete keyInput;
        usleep(100000);
        app->exit();
    }
}

int main(int argc, char *argv[])
{
    app = new QCoreApplication(argc, argv);

    keyInput = new KeyInput();
    QObject::connect(keyInput, &KeyInput::KeyPressed, keyInputSlot);
    keyInput->start();

    ftsensor = new FTSensor();
    ftsensor->start();

    app->exec();

    delete ftsensor;
    usleep(100000);

    return 0;
}
