#pragma once
#include "ObservableVec.h"

//Needs to have a forward reference due to 
//circular reference between Observer and Subject
class ObservableVec;

class ObserverVec {
public:
	~ObserverVec();
	virtual void Update() = 0;
protected:
	ObserverVec();
};


