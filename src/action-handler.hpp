#include <QTimer>
#include <queue>
#include <string>
#include <chrono>
#include <obs.hpp>
#include <map>

#include "plugin-support.h"

using namespace std;

enum ActionType {
	START_SOURCE,
	PAUSE_SOURCE,
};

typedef struct Action {
	long long time;
	ActionType type;
	void *data;
} Action;
bool operator<(const Action &a, const Action &b);

class ActionHandler : public QObject {
	Q_OBJECT

private:
	void playPauseMedia(const Action &action);

    long long getCurrentTime();

public:
	priority_queue<Action> action_queue;
	QTimer *timer;
    long long nextWakeTime;

	ActionHandler();

	void addItem(Action action);

public slots:
	void update();
};
