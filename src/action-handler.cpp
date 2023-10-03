#include "action-handler.hpp"

bool operator<(const Action &a, const Action &b)
{
	return a.time > b.time;
}

ActionHandler::ActionHandler()
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	// timer->setSingleShot(true);
	timer->start(1);
}

void ActionHandler::playPauseMedia(const Action &action)
{
	string *name = (string *)action.data;
	bool pause = (action.type == ActionType::PAUSE_SOURCE);

	obs_source *source = obs_get_source_by_name((*name).c_str());

	obs_log(LOG_DEBUG, "FOUND_SOURCE %x, SEARCHING: %s", source, name);

	obs_source_media_play_pause(source, pause);
}

long long ActionHandler::getCurrentTime()
{
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch());
	return ms.count();
}

void ActionHandler::addItem(Action action)
{
	action_queue.push(action);

	// if (!timer->isActive() || action.time < nextWakeTime) {
	// 	long long currentTime = getCurrentTime();
	// 	// timer->stop();
	// 	nextWakeTime = action.time;
	// 	timer->start(nextWakeTime - currentTime);

	//     obs_log(LOG_WARNING, "TIMER STARTED FOR %d", nextWakeTime - currentTime);
	// }
	obs_log(LOG_DEBUG, "ITEM ADDED");
}

void ActionHandler::update(void)
{
	if (action_queue.empty()) {
		return;
	}

	while (!action_queue.empty() &&
	       getCurrentTime() > action_queue.top().time) {
		obs_log(LOG_DEBUG, "ACTION TIME REACHED");
		playPauseMedia(action_queue.top());
		action_queue.pop();
	}
}