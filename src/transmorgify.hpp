// Copyright (c) 2014, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#ifndef NIXPY_TRANSMORGIFY_H
#define NIXPY_TRANSMORGIFY_H

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>


namespace nixpy {

// TODO transmorgify time_t;

template<typename T>
struct vector_transmogrify {
    static PyObject* convert(const std::vector<T>& vec) {
        boost::python::list l = boost::python::list();

        for(auto& item : vec) {
            l.append(item);
        }

        return boost::python::incref(l.ptr());
    }

    typedef boost::python::converter::rvalue_from_python_stage1_data py_s1_data;
    typedef boost::python::converter::rvalue_from_python_storage<std::vector<T>> py_storage;

    static void register_from_python() {
        boost::python::converter::registry::push_back(is_convertible,
                                                      construct,
                                                      boost::python::type_id<std::vector<T>>());
    }

    static void* is_convertible(PyObject* obj){
        return (PySequence_Check(obj) && PyObject_HasAttrString(obj,"__len__")) ? obj : nullptr;
    }

    static void construct(PyObject* obj, py_s1_data* data) {
        using namespace boost::python;

        void *raw = static_cast<void *>(reinterpret_cast<py_storage *>(data)->storage.bytes);

        int length = PySequence_Size(obj);
        new (raw) std::vector<T>(length);

        std::vector<T>* vec= static_cast<std::vector<T> *>(raw);

        for(size_t index = 0; index < length; index++) {
            vec->push_back(extract<T>(PySequence_GetItem(obj, index)));
        }

        data->convertible = raw;
    }
};

template<typename T>
struct option_transmogrify {
    static PyObject* convert(const boost::optional<T>& opt) {
        if (opt == boost::none) {
            Py_RETURN_NONE;
        }

        return boost::python::incref(boost::python::object(*opt).ptr());
    }

    typedef boost::python::converter::rvalue_from_python_stage1_data py_s1_data;
    typedef boost::python::converter::rvalue_from_python_storage<boost::optional<T>> py_storage;

    static void register_from_python() {
        boost::python::converter::registry::push_back(is_convertible,
                                                        construct,
                                                        boost::python::type_id<boost::optional<T>>());
    }

    static void* is_convertible(PyObject *obj) {
        using namespace boost::python;

        bool ok = (obj == Py_None);

        if (!ok) {
            extract<T> extractor(obj);
            ok = extractor.check();
        }

        return ok ? obj : nullptr;
    }

    static void construct(PyObject *obj, py_s1_data *data) {
        using namespace boost::python;

        void *raw = static_cast<void *>(reinterpret_cast<py_storage *>(data)->storage.bytes);

        if (obj == Py_None) {
            new (raw) boost::optional<T>{};
        } else {
            new (raw) boost::optional<T>(extract<T>(obj));
        }
        data->convertible = raw;
    }
};

}

#endif // NIXPY_TRANSMORGIFY_H