/*
 * application.cpp
 *
 *  Created on: 2011-12-5
 *      Author: lxyfirst@yahoo.com.cn
 */

#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>

#include "application.h"
#include "system_util.h"

namespace kingnet {
application::application() :
		tick_ms(get_tsc_us() * 1000), config_file(NULL), work_path(NULL), max_open_fd(
				get_open_file_limit()), m_interval_ms(DEFAULT_INTERVAL), m_delay_ms(
				0), m_daemon(0), m_status(STATUS_STOP) {
	m_timer.set_owner(this);
	m_delay_timer.set_owner(this);
}

application::~application() {

}

void application::show_help(const char* argv0) {
	printf("usage:%s -hvd  [-c filename] [-w path]\n", argv0);
	printf("options:\n"
			"-h          :show help\n"
			"-v          :show version\n"
			"-d          :run as daemon\n"
			"-c filename :set config file\n"
			"-w path     :set work directory\n");

}

void application::show_version(const char* argv0) {
	printf("%s\n", version());
}

int application::parse_option(int argc, char** argv) {

	for (int i = 1; i < argc; ++i) {
		const char* p = argv[i];
		if (*p++ != '-')
			error_return(-1,"invalid option:%s",argv[i])
			;
		switch (*p++) {
		case '\0':
			continue;
		case 'h':
			show_help(argv[0]);
			exit(0);
			break;
		case 'v':
			show_version(argv[0]);
			exit(0);
			break;
		case 'd':
			m_daemon = 1;
			break;
		case 'c':
			if (*p)
				config_file = (const char*) p;
			else if (argv[++i])
				config_file = argv[i];
			else
				error_return(-1,"option -c require parameter")
				;

			break;
		case 'w':
			if (*p)
				work_path = (const char*) p;
			else if (argv[++i])
				work_path = argv[i];
			else
				error_return(-1,"option -w require parameter")
				;

			break;

		default:
			error_return(-1,"invalid option:%s",argv[i])
			;
		}

	}

	if (config_file == NULL)
		error_return(-1,"require config file")
		;

	return 0;
}

void application::on_timeout(timer_manager* manager) {
	on_timer();

	m_timer.set_expired(get_run_ms() + m_interval_ms);
	manager->add_timer(&m_timer);

}

void application::send_signal(int signo) {
	on_signal(signo);
}

int application::set_timer(int interval_ms) {
	if (interval_ms < 10 || interval_ms > DEFAULT_INTERVAL * 10) {
		return -1;
	}

	m_interval_ms = interval_ms;

	return 0;
}

int application::set_delay_stop(int interval_ms) {
	if (interval_ms < 0 || interval_ms > DEFAULT_INTERVAL * 10) {
		return -1;
	}

	m_delay_ms = interval_ms;

	return 0;
}

void application::on_signal(int signo) {
	switch (signo) {
	case SIGINT:
	case SIGTERM:
	case SIGQUIT:
	case SIGUSR2:
		set_status(STATUS_PRE_STOP);
		break;
	case SIGUSR1:
		set_status(STATUS_RELOAD);
		break;
	default:
		;
	}

}

void application::change_work_directory(const char* argv0) {

	if (work_path)
		chdir(work_path);
	else {
		char* cmd = strdup(argv0);
		if (cmd) {
			chdir(dirname(cmd));
			free(cmd);
		}

	}

}

int application::start(int argc, char** argv) {
	if (parse_option(argc, argv) != 0)
		error_return(-1,"parse_option failed")
		;

	change_work_directory(argv[0]);

	if (m_daemon)
		daemon_init(1, 1);

	if (event_engine.init(max_open_fd) != 0) {
		error_return(-1,"init epoll_reactor failed")
		;
	}

	if (timer_engine.init(get_run_ms(), 14) != 0) {
		error_return(-1, "init timer_manager failed");
	}

	if (on_init() != 0)
		error_return(-1,"on_init failed")
		;

	m_status = STATUS_RUN;
	printf("system started\n");
	if (m_daemon) {
		int fd = open("/dev/null", O_RDWR);
		if (fd != -1) {
			dup2(fd, 0);
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}
	}

	on_timeout(&timer_engine);
	while (1) {
		int64_t now_ms = get_run_ms();
		timer_engine.run_until(now_ms);

		event_engine.run_once(timer_engine.get_next_expired() - now_ms + 2);

		if (m_status == STATUS_RELOAD) {
			on_reload();
			m_status = STATUS_RUN;
		} else if (m_status == STATUS_PRE_STOP) {
			timer_engine.del_timer(&m_timer);
			on_delay_stop();
			if (m_delay_ms < 10) {
				m_status = STATUS_STOP;
			} else {
				m_status = STATUS_RUN;
				m_delay_timer.set_expired(get_run_ms() + m_delay_ms);
				timer_engine.add_timer(&m_delay_timer);
			}
		} else if (m_status == STATUS_STOP) {
			break;
		}
	}

	on_fini();
	printf("system stopped\n");

	return 0;
}

}
