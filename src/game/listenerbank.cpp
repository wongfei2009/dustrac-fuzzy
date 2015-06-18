#include "listenerbank.hpp"
#include <MCException>

ListenerBank& ListenerBank::instance()
{
	static ListenerBank listenerBank;
	return listenerBank;
}

void ListenerBank::add(unsigned int carNum, const ListenerPtr& listener) {
	if(carNum >= size()) throw MCException("No such car in the ListenerBank. You may have forgotten to call resize.");
	m_register[carNum].insert(listener);
}

void ListenerBank::remove(unsigned int carNum, const ListenerPtr& listener) {
	if(carNum < size()) {
		auto iter = m_register[carNum].find(listener);
		if(iter != m_register[carNum].end()) m_register[carNum].erase(iter);
	}
}

void ListenerBank::clear(unsigned int carNum) {
	if(carNum < size()) m_register[carNum].clear();
}

void ListenerBank::clear() {
	m_register.clear();
}

const std::set<ListenerPtr>& ListenerBank::getListeners(unsigned int carNum) const {
	if(carNum < size()) {
		return m_register[carNum];
	} else return m_dummySet;
}

