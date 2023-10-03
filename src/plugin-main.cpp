/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <plugin-support.h>
#include <obs-websocket-api.h>
#include <map>

#include "action-handler.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

static obs_websocket_vendor vendor;
static ActionHandler *action_handler;

map<string, ActionType> action_type_map{{"start_media", START_SOURCE},
					{"pause_media", PAUSE_SOURCE}};

void handle_schedule_request(obs_data_t *request_data, obs_data_t *d, void *s)
{
	const char *request_data_json = obs_data_get_json_pretty(request_data);
	obs_log(LOG_ERROR, request_data_json);

	string type_str = string(obs_data_get_string(request_data, "type"));
	string time_str = string(obs_data_get_string(request_data, "time"));

	ActionType type = action_type_map[type_str];
	long long time = stoll(time_str);

	switch (type) {
	case START_SOURCE: {
		const char *data_str =
			obs_data_get_string(request_data, "data");
		action_handler->addItem({time, type, (void *)data_str});
		break;
	}
	case PAUSE_SOURCE: {
		const char *data_str =
			obs_data_get_string(request_data, "data");
		action_handler->addItem({time, type, (void *)data_str});
		break;
	}
	default:
		break;
	}
}

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)",
		PLUGIN_VERSION);
	action_handler = new ActionHandler();
	return true;
}

void obs_module_post_load(void)
{
	vendor = obs_websocket_register_vendor("obs-action-scheduler");
	obs_websocket_vendor_register_request(vendor, "schedule",
					      handle_schedule_request, NULL);
	obs_log(LOG_ERROR, "action-scheduler vendor loaded");
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}
