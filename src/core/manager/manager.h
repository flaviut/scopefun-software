////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __CORE__MANAGER__
#define __CORE__MANAGER__

////////////////////////////////////////////////////////////////////////////////
//
// manager
//
////////////////////////////////////////////////////////////////////////////////
class Manager {
public:
   virtual int start()          {return 0;};
   virtual int update(float dt) {return 0;};
   virtual int stop()           {return 0;};
public:
   virtual int onEvent(SDL_Event& event){return 0;};
};

////////////////////////////////////////////////////////////////////////////////
//
// manager manager
//
////////////////////////////////////////////////////////////////////////////////
class ManagerManager {
private:
   Map<String,Manager*,64>     mManager;
   Array<Manager*,64>        aStart;
   Array<Manager*,64>        aUpdate;
   Array<Manager*,64>        aStop;
public:
   int registerManager(const String name,Manager *mng);
public:
   void addStart(const String name);
   void addUpdate(const String name);
   void addStop(const String name);
public:
   void start();
   void update(float dt);
   void onEvent(SDL_Event& event);
   void stop();
};

////////////////////////////////////////////////////////////////////////////////
// macros
////////////////////////////////////////////////////////////////////////////////
#define MANAGER_POINTER(name) extern name##Manager* p##name

#define MANAGER_REGISTER(name) name##Manager* p##name = 0; \
                               void create##name () { p##name = new name##Manager(); pManager->registerManager( #name, (Manager*)p##name ); }

#define MANAGER_CREATE(name) extern void create##name(); create##name();

////////////////////////////////////////////////////////////////////////////////
// manager
////////////////////////////////////////////////////////////////////////////////
MANAGER_POINTER( Manager );

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
