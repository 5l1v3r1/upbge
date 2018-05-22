/*
 * Value.h: interface for the EXP_Value class.
 * Copyright (c) 1996-2000 Erwin Coumans <coockie@acm.org>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Erwin Coumans makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/** \file EXP_Value.h
 *  \ingroup expressions
 */

#ifndef __EXP_VALUE_H__
#define __EXP_VALUE_H__

#include "EXP_PyObjectPlus.h"

#include "CM_RefCount.h"

#ifdef WITH_PYTHON
#  include "object.h"
#endif

#include <map>
#include <vector>
#include <string>

enum VALUE_DATA_TYPE {
	VALUE_NO_TYPE, // Abstract baseclass.
	VALUE_INT_TYPE,
	VALUE_FLOAT_TYPE,
	VALUE_STRING_TYPE,
	VALUE_BOOL_TYPE,
	VALUE_LIST_TYPE,
	VALUE_MAX_TYPE // Only here to provide number of types.
};

/**
 * Baseclass EXP_Value
 *
 * Base class for all editor functionality, flexible object type that allows
 * calculations and uses reference counting for memory management.
 *
 * Features:
 * - Calculations (Calc() / CalcFinal())
 * - Property system (SetProperty() / GetProperty() / FindIdentifier())
 * - Replication (GetReplica())
 * - Flags (IsError())
 *
 * - Some small editor-specific things added
 * - A helperclass CompressorArchive handles the serialization
 *
 */
class EXP_Value : public EXP_PyObjectPlus, public CM_RefCount<EXP_Value>
{
	Py_Header
public:
	EXP_Value();
	EXP_Value(const EXP_Value& other);
	virtual ~EXP_Value();

	EXP_Value& operator=(EXP_Value&& other) = default;

#ifdef WITH_PYTHON
	virtual PyObject *py_repr(void)
	{
		return PyUnicode_FromStdString(GetText());
	}

	virtual PyObject *ConvertValueToPython()
	{
		return nullptr;
	}

	virtual EXP_Value *ConvertPythonToValue(PyObject *pyobj, const bool do_type_exception, const char *error_prefix);

	static PyObject *pyattr_get_name(EXP_PyObjectPlus *self, const EXP_PYATTRIBUTE_DEF *attrdef);

	virtual PyObject *ConvertKeysToPython();
#endif  // WITH_PYTHON

	/// Property Management
	/** Set property <ioProperty>, overwrites and releases a previous property with the same name if needed.
	 * Stall the owning of the property.
	 */
	void SetProperty(const std::string& name, EXP_Value *ioProperty);
	/// Get pointer to a property with name <inName>, returns nullptr if there is no property named <inName>.
	EXP_Value *GetProperty(const std::string & inName) const;
	/// Remove the property named <inName>, returns true if the property was succesfully removed, false if property was not found or could not be removed.
	bool RemoveProperty(const std::string& inName);
	std::vector<std::string> GetPropertyNames() const;
	/// Clear all properties.
	void ClearProperties();

	// TODO to remove in the same time timer management is refactored.
	/// Get property number <inIndex>.
	EXP_Value *GetProperty(int inIndex) const;
	/// Get the amount of properties assiocated with this value.
	int GetPropertyCount();

	virtual std::string GetText() const;
	/// Get Prop value type.
	virtual int GetValueType() const;
	/// Check if to value are equivalent.
	virtual bool Equal(EXP_Value *other) const;

	/// Retrieve the name of the value.
	virtual std::string GetName() const = 0;
	/// Set the name of the value.
	virtual void SetName(const std::string& name);

	virtual EXP_Value *GetReplica();
	virtual void ProcessReplica();

protected:
	virtual void DestructFromPython();

private:
	/// Properties for user/game etc.
	std::map<std::string, std::unique_ptr<EXP_Value> > m_properties;
};

/// Property class base, forbid name management, EXP_Value store the names inside a property map.
class EXP_PropValue : public EXP_Value
{
public:
	virtual void SetName(const std::string& name)
	{
	}

	virtual std::string GetName() const
	{
		return "";
	}
};

#endif  // __EXP_VALUE_H__
