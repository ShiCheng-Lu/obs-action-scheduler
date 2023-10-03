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

bool play_pause_first_item(void *param, obs_source_t *source)
{
	bool *pause = (bool *)param;

	obs_log(LOG_WARNING, "FOUND SOURCE");

	obs_media_state state = obs_source_media_get_state(source);

	const char *uuid = obs_source_get_uuid(source);

	obs_log(LOG_WARNING, "STATE: %d, UUID: %s", state, uuid);

	obs_source_media_play_pause(source, *pause);

	return true;
}

void ActionHandler::playPauseMedia(const Action &action)
{
	const char *uuid = (char *)action.data;
	bool pause = (action.type == ActionType::PAUSE_SOURCE);

	obs_source *source = obs_get_source_by_uuid("2");

	obs_log(LOG_WARNING, "FOUND_SOURCE %x", source);

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
	obs_log(LOG_WARNING, "ITEM ADDED");
}

void ActionHandler::update(void)
{
	if (action_queue.empty()) {
		return;
	}

	while (!action_queue.empty() &&
	       getCurrentTime() > action_queue.top().time) {
		obs_log(LOG_WARNING, "ACTION TIME REACHED");
		bool pause;
		switch (action_queue.top().type) {
		case ActionType::START_SOURCE: {
			// playPauseMedia(action_queue.top());
			pause = false;
			obs_enum_sources(play_pause_first_item, &pause);
			break;
		}

		case ActionType::PAUSE_SOURCE: {
			// playPauseMedia(action_queue.top());
			pause = true;
			obs_enum_sources(play_pause_first_item, &pause);
			break;
		}
		default:
			break;
		}

		action_queue.pop();
	}
}