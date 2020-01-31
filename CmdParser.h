#pragma once
#include <string>
#include <vector>
#include <algorithm>

////////////////
//////////////
//////////
//////

//taked from https://raw.githubusercontent.com/marcosd4h/sysmonx/master/src/common/LibCommon/cmdparser.h

//////
//////////
//////////////
////////////////
class CmdArgsParser
{

public:

	enum CaseMode
	{
		CASE_MODE_SENSITIVE = 0,
		CASE_MODE_INSENSITIVE,
		CASE_MODE_NA
	};

	CmdArgsParser() = default;
	~CmdArgsParser() = default;

	bool Initialize(int argc, char* argv[])
	{
		bool ret = false;

		for (int i = 1; i < argc; ++i)
		{
			std::string workingStr(argv[i]);
			this->m_cmdTokens.push_back(workingStr);
			std::transform(workingStr.begin(), workingStr.end(), workingStr.begin(), ::tolower);
			this->m_cmdITokens.push_back(workingStr);
		}

		if (!this->m_cmdTokens.empty())
		{
			ret = true;
		}

		return ret;
	}

	bool GetOptionValue(const std::string& cmdOption, std::string& value, int mode = CaseMode::CASE_MODE_SENSITIVE) const
	{
		bool ret = false;
		std::vector<std::string>* tokensCollection = nullptr;
		std::vector<std::string>::const_iterator cmdIt;

		if (!cmdOption.empty())
		{

			value.clear();
			std::string workingOptionStr(cmdOption);

			if (mode == CaseMode::CASE_MODE_SENSITIVE)
			{
				tokensCollection = (std::vector<std::string>*) & (this->m_cmdTokens);
			}
			else
			{
				std::transform(workingOptionStr.begin(), workingOptionStr.end(), workingOptionStr.begin(), ::tolower);
				tokensCollection = (std::vector<std::string>*) & (this->m_cmdITokens);
			}

			cmdIt = std::find((*tokensCollection).begin(), (*tokensCollection).end(), workingOptionStr);
			if (cmdIt != (*tokensCollection).end())
			{
				if (++cmdIt != (*tokensCollection).end())
				{
					std::string workArgValue(*cmdIt);

					if (workArgValue[0] != '-')
					{
						value.assign(workArgValue);
					}
				}

				ret = true;
			}
		}

		return ret;
	}

	const bool GetTwoOptionValue(const std::string& cmdOption, std::string& value1, std::string& value2, int mode = CaseMode::CASE_MODE_SENSITIVE) const
	{
		bool ret = false;
		std::vector<std::string>* tokensCollection = nullptr;
		std::vector<std::string>::const_iterator cmdIt;
		value1.clear();
		value2.clear();

		if (!cmdOption.empty())
		{
			std::string workingOptionStr(cmdOption);

			if (mode == CaseMode::CASE_MODE_SENSITIVE)
			{
				tokensCollection = (std::vector<std::string>*) & (this->m_cmdTokens);
			}
			else
			{
				std::transform(workingOptionStr.begin(), workingOptionStr.end(), workingOptionStr.begin(), ::tolower);
				tokensCollection = (std::vector<std::string>*) & (this->m_cmdITokens);
			}

			cmdIt = std::find((*tokensCollection).begin(), (*tokensCollection).end(), workingOptionStr);

			if (cmdIt != (*tokensCollection).end() && ++cmdIt != (*tokensCollection).end())
			{
				std::string workValue1(*cmdIt);
				if (!workValue1.empty() && (workValue1[0] != '-'))
				{
					value1.assign(workValue1);
					ret = true;

					if (cmdIt != (*tokensCollection).end() && ++cmdIt != (*tokensCollection).end())
					{
						std::string workValue2(*cmdIt);

						if (!workValue2.empty() && (workValue2[0] != '-'))
						{
							value2.assign(workValue2);
						}
					}

				}
			}
		}

		return ret;
	}

	bool WasOptionRequested(const std::string& cmdOption) const
	{
		return std::find(this->m_cmdTokens.begin(), this->m_cmdTokens.end(), cmdOption) != this->m_cmdTokens.end();
	}

	bool WasOptionRequestedInsensitive(const std::string& cmdOption) const
	{
		std::string workingStr(cmdOption);
		std::transform(workingStr.begin(), workingStr.end(), workingStr.begin(), ::tolower);
		return std::find(this->m_cmdITokens.begin(), this->m_cmdITokens.end(), workingStr) != this->m_cmdITokens.end();
	}

private:
	std::vector <std::string> m_cmdTokens;
	std::vector <std::string> m_cmdITokens;
};