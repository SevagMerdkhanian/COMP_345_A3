#pragma once
#include "ObserverVec.h"
#include <vector>

using namespace std;

// Forward declaration due to circular reference between Observer and Observable
class ObserverVec;

class ObservableVec {
public:
    virtual void Attach(ObserverVec* o);
    virtual void Detach(ObserverVec* o);
    virtual void Notify();
    ObservableVec();
private:
    vector<ObserverVec*>* observers;
};


