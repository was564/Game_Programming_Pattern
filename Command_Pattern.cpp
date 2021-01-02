#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// 
class ExpressionTransformedString {
public:
	static string& getTypeName(string& input) {
		for(int i=input.size(); i>=0 ; i--) {
			if (input[i] <= '9' && input[i] >= '0') {
				input.erase(input.begin() + i);
			}
		}
		return input;
	}
};

// Class created to apply the command below
// 아래 커맨드를 적용시키기 위해 만든 클래스
class GameObject {
private:
	string name;
public:
	GameObject(const string& name) : name(name) {}
	virtual ~GameObject() {}
	void jump() { cout << this->name << " " << "Jump!" << endl; }
	void fireGun() { cout << this->name << " " << "Fire!" << endl; }
	const string& getName() {
		return name;
	}
};

class Command {
public:
	Command() {}
	virtual ~Command() {}
	// Child class should implement this function. (interface)
	// 이 클래스를 상속하는 모든 자식 클래스는 이 함수를 구현하여야 한다. (인터페이스)
	virtual void execute(GameObject& object) = 0;
};

// Implement classes for each behavior
// 각 행동 별 클래스들을 구현
class JumpCommand : public Command {
private:
	void jump() {}
public:
	JumpCommand() : Command() {}
	virtual ~JumpCommand() {}
	
	void execute(GameObject& object) { object.jump(); }
};

class FireCommand : public Command {
private:
	void fireGun() {}
public:
	FireCommand() : Command() {}
	virtual ~FireCommand() {}
	
	void execute(GameObject& object) { object.fireGun(); }
};

// Null Object Pattern (for not using null)
// Null 객체 패턴 (null 사용을 없애기 위함)
class NullCommand : public Command{
public:
	NullCommand() : Command() {}
	virtual ~NullCommand() {}
	
	void execute(GameObject& object) { cout << " There is no a command" << endl; }
};

// It's easy to think of it as an input-driven class.
// 입력을 받고 실행하는 클래스라고 생각하면 쉽다.
class InputHandler {
private:
	// This variable is stored by upcasting an action command class to a command class.
	// 이 변수들은 행동 command 클래스를 Command 클래스로 업캐스팅 하여 저장한다.
	Command* buttonA;
	Command* buttonS;
	Command* nullButton;
public:
	InputHandler() :
		buttonA(new FireCommand()), 
		buttonS(new JumpCommand()),
		nullButton(new NullCommand()) {}
	virtual ~InputHandler() {
		delete buttonA;
		delete buttonS;
		delete nullButton;
	}
	// Input was received as a function for smooth execution.
	// 원활한 실행을 위해 입력을 함수로 받았다.
	Command* handleInput() {
		char input;
		cout << "press the key : ";
		cin >> input;
		if(input == 'a') return buttonA;
		if(input == 's') return buttonS;
		return nullButton;
	}
	// I think it can be implemented like the code below.
	// 아래 코드처럼 구현해도 될 것 같다. (아마 GameObject의 변경이 가능할 수도 있어서 안 쓰는듯)
	void handleInputSubstitution(GameObject& object) {
		char input;
		cout << "press the key : ";
		cin >> input;
		if(input == 'a') buttonA -> execute(object);
		if(input == 's') buttonS -> execute(object);
	}
	// swap the role of buttons. (Arbirary function)
	// 버튼들의 역할을 바꿈. (임의로 만든 함수)
	void swapButton() {
		// It is recommended that the array that manages buttons be managed static member variable.
		// 아래 코드와 같이 버튼들을 관리하는 배열은 static 멤버 변수로 관리하는게 좋다.
		
		// double pointer was used to use swap function.
		// 더블포인터는 swap 함수를 쓰기 위해 썼다.
		vector<Command**> buttonsArray;
		buttonsArray.push_back(&buttonA);
		buttonsArray.push_back(&buttonS);
		
		char selectingButtonType;
		cout << "select Button : ";
		cin >> selectingButtonType;
		
		string selectingCommand;
		cout << "select Command : ";
		cin >> selectingCommand;
		
		Command** selectingButton;
		switch(selectingButtonType) {
			case 'a':
				selectingButton = &buttonA;
				break;
			case 's':
				selectingButton = &buttonS;
				break;
			default:
				cout << "There is no a Button." << endl;
				return;
		}
		
		string applyingCommand;
		map<string, int> commands;
		commands["jump"] = 0;
		commands["fire"] = 1;
		map<string, int>::iterator command = commands.find(selectingCommand);
		if(command == commands.end()) {
			cout << "There is no a command." << endl;
			return;
		}
		switch(command->second){
			case 0:
				applyingCommand = typeid(JumpCommand).name();
				break;
			case 1:
				applyingCommand = typeid(FireCommand).name();
				break;
		}
		
		ExpressionTransformedString::getTypeName(applyingCommand);
		
		for(Command** button : buttonsArray) {
			string buttonType = typeid(**button).name();
			ExpressionTransformedString::getTypeName(buttonType);
			if(buttonType == applyingCommand) {
				swap(*button, *selectingButton);
				break;
			}
		}
	}
};
// the class that manages this program (Arbirary function)
// 프로그램을 관리하는 클래스 (임의로 만든 함수)
class ProgramEngine {
private:
	map<string, int> Mode;
	vector<GameObject*> gameObjects;
	map<string, GameObject*> gameObjectsMap; // for quick find
	InputHandler inputHandler;
	GameObject* selectingGameObject;
	
public:
	ProgramEngine() : selectingGameObject(new GameObject("Player")) {
		Mode.insert(pair<string, int> ("swap", 1));
		Mode.insert(pair<string, int> ("add", 2));
		Mode.insert(pair<string, int> ("play", 3));
		Mode.insert(pair<string, int> ("stop", 4));
		Mode.insert(pair<string, int> ("select", 5));
		Mode.insert(pair<string, int> ("info", 6));
		
		gameObjects.push_back(this->selectingGameObject);
		gameObjectsMap.insert(
			pair<string, GameObject*>(
				this->selectingGameObject->getName(),
				this->selectingGameObject
			)
		);
		
	}
	
	virtual ~ProgramEngine() {
		for(GameObject* gameObject : gameObjects){
			delete gameObject;
		}
	}
	
	void play() {
		string input = "";
		bool isStop = false;
		while(!isStop) {
			cout << "Please enter the command you want. : ";
			cin >> input;
			int command = selectMode(input);
			if (command == -1) {
				continue;
			}
			switch(command){
				case 1 : // swap buttons in InputHandler
					swapCommand();
					break;
				case 2 : // add GameObject
					addCommand();
					break;
				case 3 : // play
					playCommand();
					break;
				case 4 : // stop this program
					stopCommand(isStop);
					break;
				case 5 : // select GameObject
					selectCommand();
					break;
				case 6 : // show GameObjects
					showInfoCommand();
					break;
			}
		}
		cout << "Program is End";
	}
	
protected:
	int selectMode(string& input) {
		map<string, int>::iterator command = Mode.find(input);
		if(command == Mode.end()) {
			cout << "the command is abnormal value." << endl;
			return -1;
		}
		return command->second;
	}
	
	void swapCommand() {
		inputHandler.swapButton();
	}
	
	void addCommand() {
		string name;
		cout << "Please enter the name of a GameObject : ";
		cin >> name;
		GameObject* newGameObject = new GameObject(name);
		gameObjects.push_back(newGameObject);
		gameObjectsMap.insert(
			pair<string, GameObject*>(
				newGameObject->getName(),
				newGameObject
			)
		);
	}
	void playCommand() {
		inputHandler.handleInput()->execute(*selectingGameObject);
	}
	
	void stopCommand(bool& isStop) {
		isStop = true;
	}
	
	void selectCommand() {
		string name;
		cout << "Please enter the name of the GameObject to select : ";
		cin >> name;
		map<string, GameObject*>::iterator selectingGameObject 
			= gameObjectsMap.find(name);
		if (selectingGameObject == gameObjectsMap.end()) {
			cout << "There is no a GameObject" << endl;
			return;
		}
		this->selectingGameObject = selectingGameObject->second;
	}
	
	void showInfoCommand() {
		cout << "GameObjects list" << endl;
		for(GameObject* gameObject : gameObjects) {
			cout << "1. " << gameObject->getName() << endl;
		}
		cout << "Selecting Gameobject is " 
			<< selectingGameObject->getName() << endl;
	}
};

int main() {
	ProgramEngine programEngine;
	programEngine.play();
	
	return 0;
}
