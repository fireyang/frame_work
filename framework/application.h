/*
 * application.h
 *
 *  Created on: 2011-12-5
 *      Author: lxyfirst@yahoo.com.cn
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <signal.h>
#include <stdio.h>

#include "epoll_reactor.h"
#include "timer_manager.h"
#include "time_util.h"

namespace kingnet
{

class application
{
public:
    enum
    {
        STATUS_STOP = 0 ,
        STATUS_RUN = 1 ,
        STATUS_RELOAD = 2 ,
        STATUS_PRE_STOP = 3 ,
    };

    enum
    {
        DEFAULT_INTERVAL = 600000,
    } ;


public:
    application() ;
    virtual ~application() ;

public:
    int start(int argc,char** argv) ;

    void on_timeout(timer_manager* manager) ;
    
    void on_delay_timeout(timer_manager* manager) {m_status = STATUS_STOP;} ;

    void set_status(int8_t status) { m_status = status ; } ;
    int8_t get_status() const { return m_status ; } ;

    int64_t get_run_ms() const { return rdtsc()/tick_ms ; } ;

    int set_timer(int interval_ms) ;
    int set_delay_stop(int interval_ms);

    void send_signal(int signo) ;

protected:

    /*
     * @brief get version string , implemented derived class
     */
    virtual const char* version() { return "1.0" ; } ;


    /*
     * @brief called  when initialize , implemented derived class
     * @return 0 on success , if failed ,application will exit
     */
    virtual int on_init() {return 0; };


    /*
     * @brief called when get USR1 signal , implemented derived class
     * @return 0 on success , if failed , application will exit
     */
    virtual int on_reload() { return 0 ; } ;


    /*
     * @brief called when quit , implemented derived class
     */
    virtual void on_fini() { };

    /*
     * @brief called when get signal , should return immediately
     */
    virtual void on_signal(int signo) ;


    /*
     * @brief called every minute , implemented derived class
     */
    virtual void on_timer() { } ;

    virtual void on_delay_stop() { } ;


private:

    int parse_option(int argc,char** argv) ;

    void change_work_directory(const char* argv0) ;

    static void show_help(const char* argv0) ;

    void show_version(const char* argv0)  ;
    
    application(const application& app) ;
    application& operator=(const application& app) ;

public:
    epoll_reactor event_engine ;
    timer_manager timer_engine ;
    const int64_t tick_ms ;
    const char* config_file ;
    const char* work_path ;
    const int32_t max_open_fd ;

private:
    int32_t m_interval_ms ; 
    int32_t m_delay_ms ;
    int8_t m_daemon ;
    volatile int8_t m_status ;

    template_timer<application,&application::on_timeout> m_timer ;
    template_timer<application,&application::on_delay_timeout> m_delay_timer ;


};




}

#define DECLARE_APPLICATION_INSTANCE(app_type)   \
	app_type& get_app() ;
#define IMPLEMENT_APPLICATION_INSTANCE(app_type) \
    app_type& get_app(){ static app_type app ; return app ; }

#define IMPLEMENT_MAIN()      \
    void sig_handler(int signo){get_app().send_signal(signo);}      \
    int main(int argc,char** argv){                 \
        get_app() ;                                 \
        signal(SIGINT,sig_handler) ;               \
        signal(SIGTERM,sig_handler) ;              \
        signal(SIGQUIT,sig_handler) ;              \
        signal(SIGUSR2,sig_handler) ;              \
        signal(SIGUSR1,sig_handler) ;            \
        signal(SIGPIPE, SIG_IGN);         \
        signal(SIGALRM,SIG_IGN);          \
        signal(SIGHUP,SIG_IGN);           \
        return get_app().start(argc,argv);}

#define LOG_ERR(fmt, args...) error_log_format(get_app().logger, fmt, ##args)

#define LOG_INFO(fmt, args...) info_log_format(get_app().logger, fmt, ##args)

#define LOG_DEBUG(fmt, args...) debug_log_format(get_app().logger, fmt, ##args)

#define LOG_WARN(fmt, args...) warn_log_format(get_app().logger, fmt, ##args)

#endif /* APPLICATION_H_ */
