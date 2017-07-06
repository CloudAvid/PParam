#define _GLIBCXX_USE_NANOSLEEP 1
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <pthread.h>
#include <cstddef> 
#include <mutex>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <xlist.hpp>

using namespace std;
using namespace pparam;

#define READER_THREADS  1
#define WRITER_THREADS  1
#define UPDATER_THREADS 4
#define REMOVER_THREADS 3
#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

template <typename T>
struct list_struct {
    	XList<T> *list;
    	mutex	 *mtx;
};

template <typename T>
struct signal_struct{
	XList<T>  *list;
	sigset_t  *set;
};

template <typename T>
void *list_read(void *arguments) {
	bool flag;
	XList<T>* list = (XList<T>*) arguments;
	XListIterator<T> iterator = list->begin();
	flag = true;
	while (true) {
		if (flag) {
			++iterator;
			this_thread::sleep_for(chrono::seconds(1));
			if(iterator == list->end()){
				flag = false;
				cout << "Read Forward: Finished" << endl;
			}
		} else {
			--iterator;
			this_thread::sleep_for(chrono::seconds(2));
			if(iterator == list->begin()){
				flag = true;
				cout << "Read Reverse: Finished" << endl;
			}
		}
	}
	pthread_exit(NULL);
}

template <typename T>
void *list_write(void *arguments) {
	struct list_struct<T>* liststruct = (list_struct<T>*) arguments;
	while (true) {
		liststruct->mtx->lock();
		liststruct->list->push_back(static_cast<T>(rand()));
		liststruct->mtx->unlock();
		cout << "Write Sleep" << endl;
		this_thread::sleep_for (chrono::seconds(3));
		cout << "Write Wakeup" << endl;
	}
	pthread_exit(NULL);
}

template <typename T>
void *list_update(void *arguments){
	XList<T>* list = (XList<T>*) arguments;
        XListIterator<T> iterator;
        while (true) {
                iterator = list->begin();
	        for (int i = 0; i < rand()%(static_cast<int>(list->size()) + 1)
				; i++) {
	        	iterator.operator++();
		}
		*iterator = rand();
		cout << "Update Sleep" << endl;
                this_thread::sleep_for (chrono::seconds(4));
		cout << "Update Wakeup" << endl;
        }
	pthread_exit(NULL);
}

template <typename T>
void *list_remove(void *arguments) {
	time_t times, timee;
	struct list_struct<T>* liststruct = (list_struct<T>*) arguments;
	XListIterator<T> iterator;
	while (true) {
		XListIterator<T> iterator = liststruct->list->begin();
		for (int i = 0; i < rand()%(static_cast<int>
					(liststruct->list->size()) + 1); i++) {
			iterator.operator++();
		}
		if (iterator.is_finied() || iterator.is_end()) continue;
		liststruct->mtx->lock();
		liststruct->list->xerase_prepare(iterator);
		times = time(NULL);
		liststruct->mtx->unlock();
		liststruct->list->xerase(iterator);
		iterator.fini();
		timee = time(NULL);
		if ((timee - times) > 1) {
			cout << "the erase function is bigger than 1s and is: "
			       	<< timee - times << " seconds" <<endl;
		}
		cout << "Remove Sleep" << endl;
		this_thread::sleep_for (chrono::seconds(3));
		cout << "Remove Wakeup" << endl;
		alarm(1);
	}
	pthread_exit(NULL);
}

template <typename T>
static void* sig_handler(void *argument) {
	struct signal_struct<T>* signalstruct = (signal_struct<T>*) argument;
        int s, sig;
        while (true) {

        	s = sigwait(signalstruct->set, &sig);
		if (s != 0) {
			handle_error_en(s, "sigwait");
			continue;
		}
		switch (sig) {
			case SIGQUIT:
				exit(0);
				break;
			case SIGUSR1:
			case SIGALRM:
				cout << "the list is ";
				for (XListIterator<T> iterator = 
					signalstruct->list->begin(); 
					iterator != signalstruct->list->end();
					++iterator) {

					cout << "<-->" << *iterator;
				}
				cout << "<-->" << endl;
				break;
			
		}
	}
}

int main(int argc, char ** argv) {

	vector<pthread_t> reader, writer, updater ,remover;
	if (argc == 5) {
		cout << "number of reader thread(s):" << argv[1] << 
			" writer thread(s):" << argv[2] << 
			" updater thread(s):" << argv[3] << 
			" remover thread(s):" << argv[4] << endl;
		reader.resize(atoi(argv[1]));
		writer.resize(atoi(argv[2]));
		updater.resize(atoi(argv[3]));
		remover.resize(atoi(argv[4]));
	} else {
		cout << "number of reader thread(s):" << READER_THREADS 
			<< " writer thread(s):"	<< WRITER_THREADS 
			<< " updater thread(s):" << UPDATER_THREADS
		       	<< " remover thread(s):" << REMOVER_THREADS << endl; 
		reader.resize(READER_THREADS);
		writer.resize(WRITER_THREADS);
		updater.resize(UPDATER_THREADS);
		remover.resize(REMOVER_THREADS);
	}

	//define mutex
	mutex mtx;
	//define Xlist
	XList<int> xlist;
	for (int j = 0; j < 10; j++) {
		xlist.push_back(j);
	}

	list_struct<int> list;
	list.list = &xlist;
	list.mtx = &mtx;

	signal_struct<int> sig;
	sig.list = &xlist;
	int s;
	sigset_t set;
	
	pthread_t thread;
	sigemptyset(&set);
        sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGALRM);
        s = pthread_sigmask(SIG_BLOCK, &set, NULL);
        if (s != 0)
        	handle_error_en(s, "pthread_sigmask");
	sig.set = &set;
        s = pthread_create(&thread, NULL, sig_handler<int>, (void *) &sig);
        if (s != 0)
                handle_error_en(s, "pthread_create");

	//create reader threads
	int rc = 0;
	for (vector<pthread_t>::iterator it = reader.begin(); 
			it != reader.end(); ++it) {
		cout << "list_read(): creating thread, " << endl;
                rc = pthread_create(&*it, NULL, list_read<int>, 
				(void *) &xlist);
                if (rc) {
                	cout<< "Error:unable to create read thread, " << endl;
			exit(-1);
		}
	}

	for (vector<pthread_t>::iterator it = writer.begin(); 
			it != writer.end(); ++it) {
        	cout << "list_write(): creating thread, " << endl;
		rc = pthread_create(&*it, NULL, list_write<int>,
			       	(void *) &list);
                if (rc) {
                	cout<< "Error:unable to create write thread, " << endl;
			exit(-1);
		}
	}

	for (vector<pthread_t>::iterator it = updater.begin();
		       	it != updater.end(); ++it) {
        	cout << "list_update(): creating thread, " << endl;
                rc = pthread_create(&*it, NULL, list_update<int>,
			       	(void *) &xlist);
		if(rc){
			cout<< "Error:unable to create read thread, " << endl;
			exit(-1);
		}
	}

	for (vector<pthread_t>::iterator it = remover.begin();
		       	it != remover.end(); ++it) {
        	cout << "list_remove(): creating thread, " << endl;
		rc = pthread_create(&*it, NULL, list_remove<int>, 
				(void *) &list);
                if (rc) {
                	cout<< "Error:unable to create read thread, " << endl;
			exit(-1);
		}
	}

	void *status;
	for (vector<pthread_t>::iterator it = reader.begin();
		       	it != reader.end(); ++it) {
	      	rc = pthread_join(*it, &status);		
		if (rc) {
	      		cout << "Error:unable to join reader," << rc << endl;
			exit(-1);
		}
   	}

	for (vector<pthread_t>::iterator it = writer.begin();
			it != writer.end(); ++it) {
	      	rc = pthread_join(*it, &status);		
		if (rc) {
	      		cout << "Error:unable to join writer," << rc << endl;
			exit(-1);
		}
   	}

	for (vector<pthread_t>::iterator it = updater.begin();
		       	it != updater.end(); ++it) {
	      	rc = pthread_join(*it, &status);		
		if (rc) {
	      		cout << "Error:unable to join updater," << rc << endl;
			exit(-1);
		}
   	}

	for (vector<pthread_t>::iterator it = remover.begin();
		       	it != remover.end(); ++it) {
	      	rc = pthread_join(*it, &status);		
		if (rc) {
	      		cout << "Error:unable to join remover," << rc << endl;
			exit(-1);
		}
   	}

   	cout << "Main: program exiting." << endl;
	pthread_exit(NULL);
	return 0;
}
