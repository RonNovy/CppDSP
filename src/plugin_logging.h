/* Plugin interfaces
 * Copyright (C) 2015
 * Ron S. Novy
 */
#pragma once

#include <string>
#include <vector>

#include "plugin_interface.h"

// ********************************
// **** dsp namespace
namespace dsp
{
	// ********************************
	// **** dsp::plugin class for plugin related classes and functions.
	namespace plugin
	{
		// ********************************
		// **** Simple log entry.  Each entry has a log level and a string.
		// **** This information is managed by the host.
		typedef enum LogLevel {
			kInformation, kImportant, kWarning, kError,
		} LogLevel;
		class log_entry
		{
		public:
			log_entry(int num, LogLevel _level, const std::wstring & _msg)
				: number(num), level(_level), message(_msg) {};

			int				number;
			LogLevel		level;
			std::wstring	message;
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Plugin information logging interface.
		// **** This interface is passed to the plugin so information can be passed 
		// **** from the plugin to the host and displayed to the user. 
		class host_log : public logging_functions
		{
		public:
			virtual ~host_log() {};
			virtual void information(const std::wstring &message)
			{
				log.emplace_back(++cur_num, LogLevel::kInformation, message);
			};

			virtual void important(const std::wstring &message)
			{
				log.emplace_back(++cur_num, LogLevel::kImportant, message);
			};

			virtual void warning(const std::wstring &message)
			{
				log.emplace_back(++cur_num, LogLevel::kWarning, message);
			};

			virtual void error(const std::wstring &message)
			{
				log.emplace_back(++cur_num, LogLevel::kError, message);
			};

			virtual void clear_information()
			{
				for (int i = 0; i < log.size(); ++i)
				{
					if (log[i].level == LogLevel::kInformation)
						log.erase(log.begin() + i);
				}
			};

			virtual void clear_important()
			{
				for (int i = 0; i < log.size(); ++i)
				{
					if (log[i].level == LogLevel::kImportant)
						log.erase(log.begin() + i);
				}
			};

			virtual void clear_warning()
			{
				for (int i = 0; i < log.size(); ++i)
				{
					if (log[i].level == LogLevel::kWarning)
						log.erase(log.begin() + i);
				}
			};

			virtual void clear_error()
			{
				for (int i = 0; i < log.size(); ++i)
				{
					if (log[i].level == LogLevel::kError)
						log.erase(log.begin() + i);
				}
			};

			virtual void clear_all()
			{
				log.clear();
			};

			host_log() : cur_num(0) {};

		private:
			int cur_num;
			std::vector<log_entry>	log;
		};
		// ********************************
		// ********************************
	}
	// **** End dsp::plugin_interface
	// ********************************
}
// **** End dsp namespace
// ********************************

/*	▄▄▄▄▄▄▄ ▄▄     ▄▄  ▄▄ ▄▄▄▄▄▄▄
*	█ ▄▄▄ █ ▄  ▄▄▄██  █ ▄ █ ▄▄▄ █
*	█ ███ █ ██▄█ ▄  ▀█▄▄▀ █ ███ █
*	█▄▄▄▄▄█ ▄▀▄ █ █ ▄▀█▀▄ █▄▄▄▄▄█
*	▄▄▄▄  ▄ ▄▀ ▀ ██ ▄█▀▄▀▄  ▄▄▄ ▄
*	██  ██▄█▀▀    ▄█▀▀█▀ ███▀▀▀▀▀
*	█▄█ █ ▄ █▄ █▀▀▀▀ ▄ █▀▀  ▀ ▄ ▄
*	▄▀ █ █▄▀▀ █▀▄▀▄  █▀█▀▄▀▄ █▄▄█
*	█▀▀█ █▄▄▀▀▄▄▀▀  ▄ █ ▄ ▀▄█▀ ▄█
*	▄▀▀▀ █▄▄███▄█▀ █▄█  ▄ ▄█▄▄█
*	▄▀▀█ ▄▄▄ █▄█▄  ▀█▄ ▄▄███▀█ █
*	▄▄▄▄▄▄▄ ▀█▀▄██▀ ▀▀█▄█ ▄ █▀ ▄▀
*	█ ▄▄▄ █   █ ▄ ▄▀ ▄▀ █▄▄▄█▄▄█▀
*	█ ███ █ █▀ █▀▄▀▀ ██▀▄▀ ▄▀   █
*	█▄▄▄▄▄█ ██ ▀▄ ██▄ █▄██▄▄▀▀▄█
*/
