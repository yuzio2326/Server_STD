#include "FillContent.h"
#include <algorithm>
#include <iostream>

void FFillContentTask::RunTask(Value::ConstValueIterator& InDoc, string& InOutContent)
{
	const std::string ThisClassName = (*InDoc)["name"].GetString();
	string ParentClassName;
	auto& Parents = (*InDoc)["parents"];
	for (Value::ConstValueIterator ItParent = Parents.Begin(); ItParent != Parents.End(); ++ItParent)
	{
		// First of all..the first declared parent as Super
		ParentClassName = (*ItParent)["name"]["name"].GetString();
		break;
	}

	bool bAbstractClass = false;
	auto& Meta = (*InDoc)["meta"];
	/*if (Meta.IsArray())
	{
		for (Value::ConstValueIterator ItMeta = Meta.Begin(); ItMeta != Meta.End(); ++ItMeta)
		{
			if ((*ItMeta).HasMember("abstract"))
			{
				bAbstractClass = true;
			}
		}
	}
	else */
	if (Meta.HasMember("abstract"))
	{
		bAbstractClass = true;
	}

	/*const string CDO = "inline static const " + ThisClassName + "* CDO_" + ThisClassName + " = NewCDO<" + ThisClassName
		+ ">(nullptr, L\"" + ThisClassName + "\", RF_ClassDefaultObject);";*/

	{
		const string DisableWarningStart = "#pragma warning(push)";
		const string DisableWarning = "#pragma warning(disable:4005)";
		const string GENERATED_BODY = "#define GENERATED_BODY()\\";
		InOutContent +=
			DisableWarningStart + "\n" +
			DisableWarning + "\n" +
			GENERATED_BODY + "\n";
	}

	{
		bool bDefaultConstructor = false;
		bool bObjectInitializerConstructor = false;
		const string Public = "public:\\";
		InOutContent += Public + "\n";

		if (!bAbstractClass)
		{
			// Reflection
			{
				InOutContent += "static void Reflection() {\\\n";
				{
					InOutContent += (string)"\t\t" + "reflect<" + ThisClassName + ">()\\\n";
					InOutContent += (string)"\t\t\t" + ".type(TEXT(\"" + ThisClassName + "\")_hash)\\\n";
					InOutContent += (string)"\t\t\t" + ".base<" + ParentClassName + ">()\\\n";

					// Property
					{
						const Value& Members = (*InDoc)["members"];
						const SizeType Size = Members.Size();
						for (SizeType i = Size; i > 0; --i)
						{
							const Value& Type = Members[i - 1]["type"];
							string TypeName = Type.GetString();
							if (TypeName == "property")
							{
								const Value& Property = Members[i - 1]["name"];
								const char* PropertyName = Property.GetString();
								InOutContent += (string)"\t\t\t" + ".data<&" + ThisClassName + "::" + PropertyName + ",meta::as_alias_t>"
									+ "(\"" + PropertyName + "\"_hash, make_pair(0, FProperty{ ";

								{
									// Ex. EditAnywhere
									const Value& PropertyMeta = Members[i - 1]["meta"];
									if (PropertyMeta.Size() > 0)
									{
										InOutContent += ".PropertyFlags = ";
										{
											for (int i = 0; i < PropertyMeta.Size(); ++i)
											{
												if (PropertyMeta[i] == "EditAnywhere")
												{
													InOutContent += "EPropertyFlags::EditAnywhere";
												}
												else if (PropertyMeta[i] == "VisibleAnywhere")
												{
													InOutContent += "EPropertyFlags::VisibleAnywhere";
												}
												if ((i + 1) != PropertyMeta.Size())
												{
													InOutContent += "|";
												}
											}
										}

										InOutContent += ", ";
									}
									//auto ItFind = PropertyMeta.FindMember("EditAnywhere");

								}
								{
									const Value& DataType_Type = Members[i - 1]["dataType"]["type"];
									const string strDataType_Type = DataType_Type.GetString();
									const bool bPointer = strDataType_Type == "pointer";
									const bool bTemplate = strDataType_Type == "template";
									if (bPointer || bTemplate)
									{
										if (bPointer)
										{
											InOutContent += ".PropertyType = EPropertyType::T_POINTER";
										}
										else if (bTemplate)
										{
											const Value& Template_Type_Name = Members[i - 1]["dataType"]["name"];
											const string strTemplate_Type_Name = Template_Type_Name.GetString();
											if (strTemplate_Type_Name == "TObjectPtr")
											{
												InOutContent += ".PropertyType = EPropertyType::T_OBJECT_PTR";
											}
											else if (strTemplate_Type_Name == "TEnginePtr")
											{
												InOutContent += ".PropertyType = EPropertyType::T_ENGINE_PTR";
											}
											else if (strTemplate_Type_Name == "weak_ptr")
											{
												InOutContent += ".PropertyType = EPropertyType::T_WEAK_PTR";
											}
											else
											{
												// not supported types
												assert(false);
											}
										}
										InOutContent += ", ";
										InOutContent += (string)".Name = \"" + PropertyName + "\", ";

										string strClassName;
										if (bPointer)
										{
											strClassName = Members[i - 1]["dataType"]["baseType"]["name"].GetString();
										}
										else if (bTemplate)
										{
											strClassName = Members[i - 1]["dataType"]["arguments"][0]["name"].GetString();
										}
										InOutContent += (string)".ClassName = TEXT(\"" + strClassName + "\")" + "}))\\\n";
									}
									else
									{
										const Value& DataType_Name = Members[i - 1]["dataType"]["name"];
										string strDataType_Name = DataType_Name.GetString();
										std::transform(strDataType_Name.begin(), strDataType_Name.end(), strDataType_Name.begin(), ::tolower);
										
										bool bBaseType = false;
										if (strDataType_Name == "bool")
										{
											bBaseType = true;
											InOutContent += ".PropertyType = EPropertyType::T_BOOL";
										}
										/*else if (strDataType_Name == "uint8")
										{
											bBaseType = true;
											InOutContent += ".PropertyType = EPropertyType::T_UINT8";
										}*/
										else if (strDataType_Name == "int")
										{
											bBaseType = true;
											InOutContent += ".PropertyType = EPropertyType::T_INT";
										}
										else if (strDataType_Name == "float")
										{
											bBaseType = true;
											InOutContent += ".PropertyType = EPropertyType::T_FLOAT";
										}
										else if (strDataType_Name == "fvector")
										{
											InOutContent += ".PropertyType = EPropertyType::T_FVECTOR";
										}
										else if (strDataType_Name == "frotator")
										{
											InOutContent += ".PropertyType = EPropertyType::T_FROTATOR";
										}
										InOutContent += ", ";
										InOutContent += (string)".Name = \"" + PropertyName + "\",";
										if (bBaseType)
										{
											InOutContent += (string)".bBaseType = true,";
										}
										else
										{
											InOutContent += (string)".bBaseType = false,";
										}

										InOutContent += ".PropertySize = sizeof(" + string(PropertyName) + ")";

										InOutContent += "}))\\\n";
									}
								}
							}
							else if (TypeName == "default constructor")
							{
								bDefaultConstructor = true;
							}
							else if (TypeName == "object initializer constructor")
							{
								bObjectInitializerConstructor = true;
							}
						}
					}

					InOutContent += (string)"\t\t\t" + ";\\\n";

					//InOutContent += (string)"\t\t});\\\n";
				}

				InOutContent += (string)"\t" + "}\\\n";
			}

			{
				// __DefaultConstructor
				if (bObjectInitializerConstructor)
				{
					const string __DefaultInitializerConstructor = "DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(" + ThisClassName + ")\\\n";
					InOutContent += __DefaultInitializerConstructor;
				}
				else
				{
					const string __DefaultConstructor = "DEFINE_DEFAULT_CONSTRUCTOR_CALL(" + ThisClassName + ")\\\n";
					InOutContent += __DefaultConstructor;
				}

				// if no constructors
				if (/*bObjectInitializerConstructor == false &&*/ bDefaultConstructor == false)
				{
					// make default constructor
					InOutContent += ThisClassName + "() {}\\\n";
				}

				// delete copy constructor
				InOutContent += ThisClassName + "(const " + ThisClassName + "&) = delete;\\\n";
				InOutContent += ThisClassName + "(const " + ThisClassName + "&&) = delete;\\\n";
				InOutContent += ThisClassName + "& operator=(const " + ThisClassName + "&) = delete;\\\n";
				InOutContent += ThisClassName + "& operator=(const " + ThisClassName + "&&) = delete;\\\n";

				// StaticClass
				InOutContent += "static UClass * StaticClass() { return " + ThisClassName + "RegisterEngineClass;}\\\n";

				// Register Class
				const string RegisterClass = (string)"static inline UClass* " + ThisClassName + "RegisterEngineClass = \\\n"
					+ "\TGetPrivateStaticClassBody<" + ThisClassName + ">(\\\n"
					+ "\t\tTEXT(\"" + ThisClassName + "\"),\\\n"
					+ "\t\tInternalConstructor<" + ThisClassName + ">,\\\n"
					+ "\t\t&" + ParentClassName + "::StaticClass, \\\n"
					+ "\t\t" + ThisClassName + "::Reflection" + " \\\n" // Reflection
					+ "\t" + ");\\\n";
				InOutContent += RegisterClass;
			}
		}

		{
			const string Private = "private:\\";
			const string This = "using ThisClass = " + ThisClassName + ";" + "\\";
			const string Super = "using Super = " + ParentClassName + ";" + "\\";

			InOutContent += Private + "\n" +
				"\t" + This + "\n" +
				"\t" + Super + "\n"
				;
		}
	}

	{
		InOutContent += "\n";
		const string DisableWarningEnd = "#pragma warning(pop)";
		InOutContent += DisableWarningEnd;
		InOutContent += "\n";
	}
}