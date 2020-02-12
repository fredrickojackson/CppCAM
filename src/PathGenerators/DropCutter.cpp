/* -*- coding: utf-8 -*-

Copyright 2014 Lode Leroy

This file is part of CppCAM.

CppCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CppCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CppCAM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "DropCutter.h"
#include "PathProcessors.h"

#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include <stdio.h>
#include <exception>

class Worker : public QThread {
public:
    Worker(DropCutterImpl& dropcutter, size_t id);
    DropCutterImpl& m_dropcutter;
    size_t m_threadid;
    void run();
};

class DropCutterImpl : public QThread {
public:
    DropCutterImpl() {
        m_cutter = NULL;
        m_model = NULL;
        m_heightfield = NULL;

        m_numthreads = 3;
        m_line = 0;
        m_count = 0;
        m_finished = false;
    }

    void run()
    {
        std::vector<QThread*> threads;
        for(size_t i=0; i<m_numthreads; i++) {
            threads.push_back(new Worker(*this, i));
        }
        for(size_t i=0; i<m_numthreads; i++) {
            threads[i]->wait();
        }
        m_finished = true;
    }

    bool GetLine(size_t /*threadid*/, size_t& line) {
        QMutexLocker lock(&m_mutex);
        if (m_line < m_heightfield->height()) {
            line = m_line++;
            //printf("Thread %d calculates line %d\n", threadid, line);
            return true;
        }
        //printf("Thread %d has no more work\n", threadid);
        return false;
    }

    double progress()
    {
        return (100.0*m_count)/m_heightfield->height();
    }

    void Finished(size_t /*line*/) { 
        QMutexLocker lock(&m_mutex);
        m_count++;
        //printf("Heightfield %.1f%% done\n", progress);
    }

    bool m_finished;
    const Cutter* m_cutter;
    const Model* m_model;
    HeightField* m_heightfield;
    size_t m_numthreads;
private:
    size_t m_count;
    size_t m_line;
    QMutex m_mutex;
};

Worker::Worker(DropCutterImpl& dropcutter, size_t id) : m_dropcutter(dropcutter), m_threadid(id)
{
    QThread::moveToThread(this);
    start();
}

void Worker::run() 
{
    size_t i;
    HeightField& heightfield = *m_dropcutter.m_heightfield;
    const Model& model = *m_dropcutter.m_model;
    const Cutter& cutter = *m_dropcutter.m_cutter;
    while(m_dropcutter.GetLine(m_threadid, i)) {
        for(size_t j=0; j<heightfield.width(); j++) {
            Point cl = Point(heightfield.x(j),heightfield.y(i),INFINITE);
            if (cutter.drop(cl, model, cl)) {
                heightfield.point(j,i) = cl.z();
            }
        }

        m_dropcutter.Finished(i);
    }
}

DropCutter::DropCutter():m_impl(NULL),m_async(false)
{
    m_impl = new DropCutterImpl();
}

DropCutter::~DropCutter()
{
    if (!m_impl->m_finished) {
        throw "DropCutter deleted while not finished!";
    }
    delete m_impl;
}

void 
DropCutter::GeneratePath(const Cutter& cutter, const Model& model, HeightField& heightfield)
{
    m_impl->m_cutter = &cutter;
    m_impl->m_model = &model;
    m_impl->m_heightfield = &heightfield;
    if (m_async) {
        m_impl->start();
    } else {
        m_impl->run();
    }
}

void 
DropCutter::setNumthreads(size_t numthreads)
{
    m_impl->m_numthreads = numthreads;
}

void 
DropCutter::setAsync(bool async)
{
    m_async = async;
}

bool 
DropCutter::finished(double* progress)
{
    if (progress) {
        *progress = m_impl->progress();
    }
    return m_impl->m_finished;
}
