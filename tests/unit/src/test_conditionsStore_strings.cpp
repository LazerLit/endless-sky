/* test_conditionsStore_strings.cpp
Copyright (c) 2024 by Nick

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "es-test.hpp"

// Include only the tested class's header.
#include "../../../source/ConditionsStore.h"

// ... and any system includes needed for the test file.
#include <map>
#include <string>



namespace { // test namespace

// #region unit tests

SCENARIO( "Setting string variables", "[ConditionsStore][StringVariables][Setting]" )
{
	GIVEN( "An empty ConditionsStore" )
	{
		auto store = ConditionsStore();

		WHEN( "a string variable is set" )
		{
			store.SetString("prime minister name", "Arthur Baker");
			THEN( "it can be retrieved by name" )
			{
				REQUIRE( store.GetString("prime minister name") == "Arthur Baker" );
			}
			THEN( "it is recognised as existing" )
			{
				REQUIRE( store.HasString("prime minister name") );
			}
			THEN( "it does not affect integer conditions" )
			{
				REQUIRE( store.Get("prime minister name") == 0 );
				REQUIRE( store.PrimariesSize() == 0 );
			}
		}

		WHEN( "a string variable is set and then updated" )
		{
			store.SetString("prime minister name", "Arthur Baker");
			store.SetString("prime minister name", "Mustafa Kikelomo");
			THEN( "the updated value is returned" )
			{
				REQUIRE( store.GetString("prime minister name") == "Mustafa Kikelomo" );
			}
		}

		WHEN( "a string variable is set to an empty string" )
		{
			store.SetString("prime minister name", "Arthur Baker");
			store.SetString("prime minister name", "");
			THEN( "the variable no longer exists" )
			{
				REQUIRE_FALSE( store.HasString("prime minister name") );
			}
			THEN( "GetString returns an empty string" )
			{
				REQUIRE( store.GetString("prime minister name") == "" );
			}
		}

		WHEN( "multiple string variables are set" )
		{
			store.SetString("prime minister name", "Arthur Baker");
			store.SetString("incumbent bloc", "Moderates");
			store.SetString("opposition bloc", "Traditionalists");
			THEN( "each can be retrieved independently" )
			{
				REQUIRE( store.GetString("prime minister name") == "Arthur Baker" );
				REQUIRE( store.GetString("incumbent bloc") == "Moderates" );
				REQUIRE( store.GetString("opposition bloc") == "Traditionalists" );
			}
			THEN( "each is recognised as existing" )
			{
				REQUIRE( store.HasString("prime minister name") );
				REQUIRE( store.HasString("incumbent bloc") );
				REQUIRE( store.HasString("opposition bloc") );
			}
		}
	}
}



SCENARIO( "Querying non-existent string variables", "[ConditionsStore][StringVariables][Missing]" )
{
	GIVEN( "An empty ConditionsStore" )
	{
		const auto store = ConditionsStore();

		WHEN( "a string variable that was never set is queried" )
		{
			THEN( "GetString returns an empty string" )
			{
				REQUIRE( store.GetString("prime minister name") == "" );
			}
			THEN( "HasString returns false" )
			{
				REQUIRE_FALSE( store.HasString("prime minister name") );
			}
			THEN( "integer conditions are not affected" )
			{
				REQUIRE( store.PrimariesSize() == 0 );
			}
		}
	}

	GIVEN( "A ConditionsStore with one string variable set" )
	{
		auto store = ConditionsStore();
		store.SetString("prime minister name", "Arthur Baker");

		WHEN( "a different string variable is queried" )
		{
			THEN( "GetString returns an empty string for the missing variable" )
			{
				REQUIRE( store.GetString("opposition leader name") == "" );
			}
			THEN( "HasString returns false for the missing variable" )
			{
				REQUIRE_FALSE( store.HasString("opposition leader name") );
			}
			THEN( "the existing variable is unaffected" )
			{
				REQUIRE( store.GetString("prime minister name") == "Arthur Baker" );
				REQUIRE( store.HasString("prime minister name") );
			}
		}
	}
}



SCENARIO( "String variables and integer conditions are independent", "[ConditionsStore][StringVariables][Isolation]" )
{
	GIVEN( "A ConditionsStore with both integer and string variables" )
	{
		auto store = ConditionsStore();
		store.Set("moderates in power", 1);
		store.Set("terms", 2);
		store.SetString("prime minister name", "Arthur Baker");
		store.SetString("incumbent bloc", "Moderates");

		WHEN( "integer conditions are queried" )
		{
			THEN( "they return the correct values" )
			{
				REQUIRE( store.Get("moderates in power") == 1 );
				REQUIRE( store.Get("terms") == 2 );
			}
		}
		WHEN( "string variables are queried" )
		{
			THEN( "they return the correct values" )
			{
				REQUIRE( store.GetString("prime minister name") == "Arthur Baker" );
				REQUIRE( store.GetString("incumbent bloc") == "Moderates" );
			}
		}
		WHEN( "an integer condition shares a name with a string variable" )
		{
			store.Set("shared name", 42);
			store.SetString("shared name", "some text");
			THEN( "Get returns the integer value" )
			{
				REQUIRE( store.Get("shared name") == 42 );
			}
			THEN( "GetString returns the string value" )
			{
				REQUIRE( store.GetString("shared name") == "some text" );
			}
			THEN( "both can be updated independently" )
			{
				store.Set("shared name", 99);
				store.SetString("shared name", "other text");
				REQUIRE( store.Get("shared name") == 99 );
				REQUIRE( store.GetString("shared name") == "other text" );
			}
		}
		WHEN( "a string variable is cleared" )
		{
			store.SetString("prime minister name", "");
			THEN( "integer conditions are unaffected" )
			{
				REQUIRE( store.Get("moderates in power") == 1 );
				REQUIRE( store.Get("terms") == 2 );
				REQUIRE( store.PrimariesSize() == 2 );
			}
			THEN( "other string variables are unaffected" )
			{
				REQUIRE( store.GetString("incumbent bloc") == "Moderates" );
				REQUIRE( store.HasString("incumbent bloc") );
			}
		}
	}
}



SCENARIO( "Adding string substitutions to a map", "[ConditionsStore][StringVariables][Substitutions]" )
{
	GIVEN( "A ConditionsStore with string variables set" )
	{
		auto store = ConditionsStore();
		store.SetString("prime minister name", "Arthur Baker");
		store.SetString("incumbent bloc", "Moderates");

		WHEN( "AddStringSubstitutions is called on an empty map" )
		{
			std::map<std::string, std::string> subs;
			store.AddStringSubstitutions(subs);
			THEN( "each string variable appears with angle-bracket wrapping" )
			{
				REQUIRE( subs["<prime minister name>"] == "Arthur Baker" );
				REQUIRE( subs["<incumbent bloc>"] == "Moderates" );
			}
			THEN( "no other entries are added" )
			{
				REQUIRE( subs.size() == 2 );
			}
		}

		WHEN( "AddStringSubstitutions is called on a map with existing entries" )
		{
			std::map<std::string, std::string> subs;
			subs["<planet>"] = "New Boston";
			subs["<payment>"] = "1,000 credits";
			store.AddStringSubstitutions(subs);
			THEN( "existing entries are preserved" )
			{
				REQUIRE( subs["<planet>"] == "New Boston" );
				REQUIRE( subs["<payment>"] == "1,000 credits" );
			}
			THEN( "string variables are added alongside them" )
			{
				REQUIRE( subs["<prime minister name>"] == "Arthur Baker" );
				REQUIRE( subs["<incumbent bloc>"] == "Moderates" );
			}
		}

		WHEN( "a string variable shares a key with an existing substitution" )
		{
			std::map<std::string, std::string> subs;
			subs["<prime minister name>"] = "old value";
			store.AddStringSubstitutions(subs);
			THEN( "the string variable overwrites the existing entry" )
			{
				REQUIRE( subs["<prime minister name>"] == "Arthur Baker" );
			}
		}
	}

	GIVEN( "A ConditionsStore with no string variables" )
	{
		const auto store = ConditionsStore();
		WHEN( "AddStringSubstitutions is called" )
		{
			std::map<std::string, std::string> subs;
			subs["<planet>"] = "New Boston";
			store.AddStringSubstitutions(subs);
			THEN( "the existing map is unchanged" )
			{
				REQUIRE( subs.size() == 1 );
				REQUIRE( subs["<planet>"] == "New Boston" );
			}
		}
	}
}

// #endregion unit tests



} // test namespace
