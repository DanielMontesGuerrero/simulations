#pragma once

#include "gameoflife/gameoflife.hpp"
#include "gameoflife/client.hpp"
#include "utilscpp/matrix.hpp"
#include <queue>
#include <deque>

using std::queue;
using std::deque;

struct Submatrix {
	int x, y;
	Matrix matrix;
};

class GameHandler {
	public:
		GameOfLife gameoflife;
		bool is_executed_locally;
		Client client;
		queue<Submatrix, deque<Submatrix>> pending_updates;

		GameHandler(int rows, int cols, bool is_executed_locally = true);
		void update(int x, int y, int w, int h);
		void update();
		void on_click(int i, int j);
		void increase_update_rate();
		void decrease_update_rate();
		void toggle_pause();
		void close_server();
	private:
		int last_x;
		int last_y;
		int last_w;
		int last_h;
};
