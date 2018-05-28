#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include "Event.h"
#include "utils.h"
#include "QueryManager.h"
#include <list>

#include "MyType.h"

#define QUEUE_CAPACITY 1000

#define random(a, b) (rand() % (b-a) + a)
#define randomInt(a, b) (rand() % b + a)
#define randomFloat(a, b) (rand() /float(RAND_MAX) * b + a)


class IncomingManager {

public:
	static void reveiveEvents(EventPtr eventPtr);

	static int bufferSize() {
		return size;
	}

	void outputEvents();

	static long eventID;

private:
	//store incoming event stream
	//static queue<Event*> eventQueue;
	static list<EventPtr> eventQueue;

	static int size;

	
};

//queue<Event*> IncomingManager::eventQueue;
list<EventPtr> IncomingManager::eventQueue;

int IncomingManager::size = 0;

long IncomingManager::eventID = 0;

//store event in the queue
void IncomingManager::reveiveEvents(EventPtr eventPtr) {
	eventQueue.push_back(eventPtr);
	size++;
	
}

//ouput the event from the queue
void IncomingManager::outputEvents() {
	EventPtr event = nullptr;
	while (true) {
		while (IncomingManager::eventQueue.empty()) {
			//cout << "consuming thread sleep..." << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		if (!eventQueue.empty()) {
			event = eventQueue.front();
			eventQueue.pop_front();
			QueryManager::matchCQs(event);
			size--;
		}
	}
}


void generateEvents() {
	cout << "start to generate events..." << endl;
	//srand((unsigned)time(NULL));

	string topicNames[] = { "car1" };

	int len = 1;
	
	//Event * msg = nullptr;
	while (true) {
		while (IncomingManager::bufferSize() > QUEUE_CAPACITY) {
			//cout << "generating thread sleep..." << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		for (int i = 0; i < len; i++) {
			boost::shared_ptr<Event> msg(new Event());
			msg->id = IncomingManager::eventID++;
			msg->direction = randomFloat(0, 360);
			msg->topicName = topicNames[0];
			msg->elevation = randomFloat(-30, 100);
			msg->speed = randomFloat(-30, 100);
			msg->longitude = randomFloat(0, 182);
			msg->latitude = randomFloat(0, 92);
			msg->time = getTime();

			//cout << *msg << endl;
			IncomingManager::reveiveEvents(msg);
		}
		this_thread::sleep_for(chrono::milliseconds(10));
		len = random(1, 7);
		//cout << "event num in the buffer: " << IncomingManager::bufferSize() << "  \n";
	}
}

void startGenerateEvent() {
	cout << "ready to generate events..." << endl;
	thread task(generateEvents);
	task.detach();
}