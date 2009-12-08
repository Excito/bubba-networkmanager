/*
    
    bubba-networkmanager - http://www.excito.com/
    
    Hosts.cpp - this file is part of bubba-networkmanager.
    
    Copyright (C) 2009 Tor Krill <tor@excito.com>
    
    bubba-networkmanager is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.
    
    bubba-networkmanager is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    version 2 along with libeutils; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
    
    $Id$
*/

#include "Hosts.h"

#include <libeutils/FileUtils.h>
#include <libeutils/StringTools.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace EUtils;

namespace NetworkManager{
Hosts::Hosts(){
	list<string> fil=FileUtils::GetContent(HOSTS_FILE);

	for(list<string>::iterator fIt=fil.begin();fIt!=fil.end();fIt++){
		Entry e;
		list<string> words=StringTools::Split(StringTools::Trimmed(*fIt," \t"),"[[:blank:]]+");
		for(list<string>::iterator word=words.begin();word!=words.end();word++){
			if((*word)[0]=='#'){
				break;
			}
			e.push_back(*word);
		}
		if(e.size()>0){
			entries.push_back(e);
		}

	}
}

void Hosts::UpdateIP(Hosts::Entries& e, const string& ip){
	for(Hosts::Entries::iterator entry=e.begin();entry!=e.end();entry++){
		if((*entry)[0].substr(0,3)=="127"){ // Skip loopback
			continue;
		}
		(*entry)[0]=ip;
	}
}

void Hosts::UpdateHostname(Hosts::Entries& e, const string& name){
	for(Hosts::Entries::iterator entry=e.begin();entry!=e.end();entry++){
		(*entry).erase((*entry).begin()+1,(*entry).end());
		(*entry).insert((*entry).end(),name);
		(*entry).insert((*entry).end(),name+".localdomain");
	}
}


static bool SearchLine(const Hosts::Entry& e, const string& term){
	bool del=false;
	for(Hosts::Entry::const_iterator word=e.begin();word!=e.end();word++){
		if((*word)==term){
			del=true;
		}
	}
	return del;
}

Hosts::Entries Hosts::Find(const string& term){
	Entries e;
	for(Entries::const_iterator entry=this->entries.begin();entry!=this->entries.end();entry++){
		if(SearchLine(*entry,term)){
			e.push_back(*entry);
		}
	}
	return e;
}

void Hosts::Delete(const string& term){
	for(Entries::iterator entry=this->entries.begin();entry!=this->entries.end();entry++){
		while(entry!=entries.end() && SearchLine(*entry,term)){
			entry=this->entries.erase(entry);
		}
	}
}

void Hosts::Add(const Entries& e){
	for(Entries::const_iterator entry=e.begin();entry!=e.end();entry++){
		this->Add(*entry);
	}
}

void Hosts::Add(const Entry& e){
	this->entries.push_back(e);
}

bool Hosts::WriteBack(){
	list<string> lines;
	for(Entries::const_iterator entry=this->entries.begin();entry!=this->entries.end();entry++){
		stringstream l;
		bool first=true;
		for(Entry::const_iterator word=(*entry).begin();word!=(*entry).end();word++){
			if(!first){
				l<< "\t";
			}else{
				first=false;
			}
			l << *word;
		}
		l<<endl;
		lines.push_back(l.str());
	}
	if(!FileUtils::Write(HOSTS_FILE".new",lines,0644)){
		return false;
	}
	if(rename(HOSTS_FILE".new",HOSTS_FILE)!=0){
		return false;
	}
	return true;

}

void Hosts::Dump(const Entries& e){
	for(Entries::const_iterator entry=e.begin();entry!=e.end();entry++){
		for(Entry::const_iterator word=(*entry).begin();word!=(*entry).end();word++){
			cout << "["<<*word<<"] ";
		}
		cout <<endl;
	}
}

void Hosts::Dump(){
	this->Dump(this->entries);
}

Hosts::~Hosts(){
}
}
