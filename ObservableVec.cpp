#include "ObservableVec.h"
#include "ObserverVec.h"
#include <vector>

ObservableVec::ObservableVec() {
    observers = new std::vector<ObserverVec*>;
}

void ObservableVec::Attach(ObserverVec* o) {
    observers->push_back(o);
}

void ObservableVec::Detach(ObserverVec* o) {
    observers->erase(std::remove(observers->begin(), observers->end(), o), observers->end());
}

void ObservableVec::Notify() {
    for (ObserverVec* observer : *observers) {
        observer->Update();
    }
}
