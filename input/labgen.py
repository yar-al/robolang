import random
import sys

# Define the dimensions of the labyrinth
WIDTH = 20
HEIGHT = 10

# Directions for movement: right, down, left, up
DIRECTIONS = [(0, 1), (1, 0), (0, -1), (-1, 0)]


def in_bounds(x, y):
    """Check if the coordinates are within the bounds of the labyrinth."""
    return 0 <= x < WIDTH and 0 <= y < HEIGHT


def generate_labyrinth():
    """Generate a rectangular labyrinth."""
    # Initialize the labyrinth with walls
    labyrinth = [['1' for _ in range(WIDTH)] for _ in range(HEIGHT)]
    for i in range(HEIGHT):
        for j in range(WIDTH):
            labyrinth[i][j] = str(random.randrange(10) // 9 + 1)
    # Start carving paths from (1, 1)
    carve_path(labyrinth, 1, 1)

    return labyrinth


def carve_path(labyrinth, x, y):
    """Carve out a path in the labyrinth using depth-first search."""
    labyrinth[y][x] = '0'  # Carve out the current cell

    # Randomize the directions
    random_directions = DIRECTIONS[:]
    random.shuffle(random_directions)

    # Explore each direction
    for dx, dy in random_directions:
        nx, ny = x + dx * 2, y + dy * 2  # Calculate the next cell

        # If the next cell is in bounds and is still a wall
        if in_bounds(nx, ny) and labyrinth[ny][nx] == '1':
            labyrinth[y + dy][x + dx] = '0'  # Carve a path to the next cell
            carve_path(labyrinth, nx, ny)  # Recur from the next cell


def print_labyrinth(labyrinth):
    """Print the generated labyrinth."""
    for row in labyrinth:
        print(''.join(row))


if __name__ == "__main__":
    f = open('map.txt', 'w')
    sys.stdout = f 
    print(1,1)
    random.seed()  # Seed for randomization
    labyrinth = generate_labyrinth()  # Generate the labyrinth
    print_labyrinth(labyrinth)  # Print the labyrinth