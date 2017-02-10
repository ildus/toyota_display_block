#define EJECT_PIN 7
#define POWER_PIN 5
#define MUTE_PIN 8
#define VOLA_PIN 3
#define VOLB_PIN 4
#define MODEA_PIN 9
#define MODEB_PIN 10
#define PWR_PIN  7
#define KEY_S0 2
#define KEY_S1 6
#define KEY_S2 A1
#define KEY_S3 13
#define KEY_D0 A4
#define KEY_D1 11
#define KEY_D2 A3
#define KEY_D3 12
#define KEY_D4 A2
#define LED_PIN A0

using namespace std;

#include <vector>

const int BOUNCE_TIME = 50;
bool blink_state = false;

struct ButtonState {
	bool clicked;
	unsigned int last_click;
	char old_level;
};

struct EncoderState {
	int position;
	char prev_a;
};

void blink() {
	blink_state = true;
}

void check_button_state(ButtonState *state, int pin) {
	char level = digitalRead(pin);

	if (level == LOW && state->old_level != level) {
		if (millis() - state->last_click > BOUNCE_TIME) {
			state->clicked = true;
			blink();
		}

		state->last_click = millis();
	}
	state->old_level = level;
}

void check_encoder_state(EncoderState *state, int pin_a, int pin_b) {
	char a = digitalRead(pin_a);
	char b = digitalRead(pin_b);

	if ((a == LOW) && (state->prev_a == HIGH)) {
		if (b == HIGH) {
			state->position += 1;
		} else {
			state->position -= 1;
		}
	}
	state->prev_a = a;
}

void check_blink(int pin, unsigned int current_time) {
	static unsigned int blink_time = 0;
	if (blink_time != 0 && current_time - blink_time > 200) {
		blink_time = 0;
		blink_state = false;
		digitalWrite(pin, 0);
	} else if (blink_state) {
		blink_state = false;
		digitalWrite(pin, 1);
		blink_time = current_time;
	}
}

struct Connection {
	int in;
	int out;
	const char *description;
};

Connection connections[] = {
	{KEY_S0, KEY_D0, "reroute"},
	{KEY_S0, KEY_D1, "guidance"},
	{KEY_S0, KEY_D2, "seek_down"},
	{KEY_S0, KEY_D3, "seek_up"},
	{KEY_S0, KEY_D4, "amfm"},
	{KEY_S1, KEY_D0, "d0"},
	{KEY_S1, KEY_D1, "d1"},
	{KEY_S1, KEY_D2, "ch3"},
	{KEY_S1, KEY_D3, "ch2"},
	{KEY_S1, KEY_D4, "ch1"},
	{KEY_S2, KEY_D0, "ad0"},
	{KEY_S2, KEY_D1, "ad1"},
	{KEY_S2, KEY_D2, "ch6"},
	{KEY_S2, KEY_D3, "ch5"},
	{KEY_S2, KEY_D4, "ch4"},
	{KEY_S3, KEY_D0, "menu"},
	{KEY_S3, KEY_D1, "back"},
	{KEY_S3, KEY_D2, "enter"},
	{KEY_S3, KEY_D3, "disc"},
	{KEY_S3, KEY_D4, "trip"}
};

int check_connection(int pin) {
	int result_pin = 0;
	int pins[] = {KEY_D0, KEY_D1, KEY_D2, KEY_D3, KEY_D4};
	digitalWrite(pin, 1);
	for (int pin : pins) {
		char val = digitalRead(pin);
		if (val) {
			result_pin = pin;
			break;
		}
	}
	digitalWrite(pin, 0);
	return result_pin;
}

void setup() {
	Serial.begin(9600);
	pinMode(LED_PIN, OUTPUT);
	pinMode(KEY_S0, OUTPUT);
	pinMode(KEY_S1, OUTPUT);
	pinMode(KEY_S2, OUTPUT);
	pinMode(KEY_S3, OUTPUT);
	blink();
	Serial.println("working...");
}

void check_connections(std::vector<Connection*> &vec) {
	static Connection *last = nullptr;
	static unsigned int last_time;
	Connection *current = nullptr;

	int pins[] = {KEY_S0, KEY_S1, KEY_S2, KEY_S3};
	for (int in : pins) {
		int out = check_connection(in);
		if (out > 0) {
			for (Connection &conn : connections) {
				if (conn.in == in && conn.out == out) {
					current = &conn;
					break;
				}
			}
			break;
		}
	}

	if (current && last != current) {
		unsigned int current_time = millis();
		if (last_time > 0 && (current_time - last_time > 50)) {
			vec.push_back(current);
			blink();
		}
		last_time = current_time;
	}

	last = current;
}

std::vector<Connection *> conns;

void loop() {
	static ButtonState eject_state, mute_state, power_state;
	static EncoderState volume_state, mode_state;
	static unsigned int last_check_time, last_process_time;
	unsigned int current_time = millis();

	if (current_time - last_check_time > 10) {
		check_button_state(&eject_state, EJECT_PIN);
		check_button_state(&mute_state, MUTE_PIN);
		check_button_state(&power_state, POWER_PIN);
		check_encoder_state(&volume_state, VOLA_PIN, VOLB_PIN);
		check_encoder_state(&mode_state, MODEA_PIN, MODEB_PIN);
		check_connections(conns);

		last_check_time = current_time;
	}

	if (current_time - last_process_time > 100) {
		if (eject_state.clicked) {
			Serial.println("eject");
			eject_state.clicked = false;
		}

		if (mute_state.clicked) {
			Serial.println("mute");
			mute_state.clicked = false;
		}

		if (power_state.clicked) {
			Serial.println("power");
			power_state.clicked = false;
		}

		if (volume_state.position != 0) {
			Serial.print("vol ");
			Serial.println(volume_state.position);
			volume_state.position = 0;
		}

		if (mode_state.position != 0) {
			Serial.print("mode ");
			Serial.println(mode_state.position);
			mode_state.position = 0;
		}

		if (conns.size() > 0) {
			for (auto conn: conns) {
				Serial.println(conn->description);
			}
			conns.clear();
		}

		last_process_time = current_time;
	}

	check_blink(A0, current_time);
}
