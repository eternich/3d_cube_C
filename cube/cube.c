#include <SDL3/SDL.h>
#include <stdio.h.>
#include <math.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define COLOR_WHITE 0xffffff
#define COLOR_BLACK 0x000000
#define NUMBER_OF_POINTS 1200
#define SIDE_LENGTH 300
#define POINT_SIZE 5
#define COORDINATE_SYSTEM_OFFSET_X  SCREEN_WIDTH / 2
#define COORDINATE_SYSTEM_OFFSET_Y  SCREEN_HEIGHT / 2


typedef struct
{
	int x;
	int y;
	int z;
}Point;

void paint_it_black(SDL_Surface* surface)
{
	SDL_Rect black_rect = (SDL_Rect){ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_FillSurfaceRect(surface, &black_rect, COLOR_BLACK);
}

int draw_point(SDL_Surface* surface, int x, int y)
{
	SDL_Rect rect = (SDL_Rect){ x, y, POINT_SIZE, POINT_SIZE };
	SDL_FillSurfaceRect(surface, &rect, COLOR_WHITE);
}

int draw_point_3d(SDL_Surface* surface, Point point)
{
	int x_2d = point.x + COORDINATE_SYSTEM_OFFSET_X;
	int y_2d = point.y + COORDINATE_SYSTEM_OFFSET_Y;
	draw_point(surface, x_2d, y_2d);
}

int draw_points_3d(SDL_Surface* surface, Point points[])
{
	for (int i = 0; i < NUMBER_OF_POINTS; i++)
	{
		int x_2d = points[i].x + COORDINATE_SYSTEM_OFFSET_X;
		int y_2d = points[i].y + COORDINATE_SYSTEM_OFFSET_Y;
		draw_point(surface, x_2d, y_2d);
	}
}

void generate_rotation_matrix(Point* point, double alpha, double beta, double gamma)
{
	double rotation_matrix[3][3] = { 
		{cos(alpha) * cos(beta), cos(alpha) * sin(beta) * sin(gamma) - sin(alpha) * cos(gamma), cos(alpha) * sin(beta) * cos(gamma) + sin(alpha) * sin(gamma)}, 
		{sin(alpha) * cos(beta), sin(alpha) * sin(beta) * sin(gamma) + cos(alpha) * cos(gamma), sin(alpha) * sin(beta) * cos(gamma) - cos(alpha) * sin(gamma)}, 
		{-sin(beta), cos(beta) * sin(gamma), cos(beta) * cos(gamma)}
	};
	int point_vector[3] = { point->x, point->y, point->z };
	int result[3] = { 0, 0, 0 };

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i] += rotation_matrix[i][j] * point_vector[j];
		}
	}

	point->x = result[0];
	point->y = result[1];
	point->z = result[2];
}

void initialize_cube(Point points[])
{
	//a cube has 12 sides
	int points_per_side = NUMBER_OF_POINTS / 12;
	int step_size = SIDE_LENGTH / points_per_side;

	//side 1
	for (int i = 0; i < points_per_side; i++)
	{
		points[i] = (Point) {-SIDE_LENGTH / 2 + i * step_size, -SIDE_LENGTH / 2, SIDE_LENGTH / 2 };
	}

	//side 2
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + points_per_side] = (Point){ -SIDE_LENGTH / 2 + i * step_size, SIDE_LENGTH / 2, SIDE_LENGTH / 2 };
	}

	//side 3
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 2 * points_per_side] = (Point){ -SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size, SIDE_LENGTH / 2 };
	}

	//side 4
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 3 * points_per_side] = (Point){ SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size, SIDE_LENGTH / 2 };
	}

	//side 5
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 4 * points_per_side] = (Point){ -SIDE_LENGTH / 2 + i * step_size, -SIDE_LENGTH / 2, -SIDE_LENGTH / 2 };
	}

	//side 6
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 5 * points_per_side] = (Point){ -SIDE_LENGTH / 2 + i * step_size, SIDE_LENGTH / 2, -SIDE_LENGTH / 2 };
	}

	//side 7
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 6 * points_per_side] = (Point){ -SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size, -SIDE_LENGTH / 2 };
	}

	//side 8
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 7 * points_per_side] = (Point){ SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size, -SIDE_LENGTH / 2 };
	}

	//side 9
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 8 * points_per_side] = (Point){ -SIDE_LENGTH / 2, -SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size};
	}

	//side 10
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 9 * points_per_side] = (Point){ SIDE_LENGTH / 2, -SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size };
	}

	//side 11
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 10 * points_per_side] = (Point){ SIDE_LENGTH / 2, SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size };
	}

	//side 12
	for (int i = 0; i < points_per_side; i++)
	{
		points[i + 11 * points_per_side] = (Point){ -SIDE_LENGTH / 2, SIDE_LENGTH / 2, -SIDE_LENGTH / 2 + i * step_size };
	}
}


int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("3DCube", SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	SDL_Surface* surface = SDL_GetWindowSurface(window);

	int simulation_running = 1;
	SDL_Event event;
	SDL_KeyboardEvent keyboard_event;
	double rotation_step = 0.05;
	int yaw = 1;
	int pitch = 1;
	int roll = 1;
	int k = 0;
	int l = 0;
	int m = 0;

	double alpha = 0.01;
	double beta = 0.01;
	double gamma = 0.01;

	while (simulation_running)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				simulation_running = 0;

			case SDL_EVENT_KEY_DOWN:
				printf("key pressed\n");
			}
		}

		Point points[NUMBER_OF_POINTS];
		initialize_cube(points);
		//draw_points_3d(surface, points);
		paint_it_black(surface);

		for (int i = 0; i < NUMBER_OF_POINTS; i++)
		{
			generate_rotation_matrix(&points[i], k * yaw * alpha, l * pitch * beta, m * roll * gamma);
			//draw_points_3d(surface, points);
		}

		draw_points_3d(surface, points); 

		SDL_UpdateWindowSurface(window);

		SDL_Delay(10);

		k++;
		l++;
		m++;
	}

}
