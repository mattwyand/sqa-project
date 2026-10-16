#pragma once
#include <string>
#include <vector>

class Program;

class Command
{
protected:
	Program *program;
	std::string name, description;

public:
	Command(Program *program, const char *name, const char *description)
		: program(program), name(name), description(description) {}

	virtual ~Command() = default;

	void print_description();
	virtual void run(const std::vector<std::string> &args) = 0;

	inline const std::string get_name() const { return name; }
};

class ExitCommand : public Command
{
public:
	ExitCommand(Program *program);
	void run(const std::vector<std::string> &args) override;
};


class LoginCommand : public Command
{
public:
	LoginCommand(Program *program);
	void run(const std::vector<std::string> &args) override;
};

class LogoutCommand : public Command
{
public:
	LogoutCommand(Program *program);
	void run(const std::vector<std::string> &args) override;
};