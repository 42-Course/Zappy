class Tile
  RESOURCE_COLORS = [
    'red',      # Food
    'orange',   # Linemate
    'yellow',   # Deraumère
    'green',    # Sibur
    '#00FFFF',     # Mendiane
    'blue',     # Phiras
    'purple'    # Thystame
  ]

  attr_reader :position
  attr_accessor :resources

  def initialize(x, y)
    @position = Point.new(x, y)
    @resources = Array.new(7, 0) # 7 resource types
  end

  def set_resources(resources)
    @resources = resources
  end

  def draw(draw_x, draw_y, tile_size, show_grid, is_selected, z = 3)
    size = tile_size - (if show_grid then 1 else 0 end)

    # Draw base tile background
    Square.new(
      x: draw_x, y: draw_y,
      size: size,
      color: is_selected ? '#0240f2' : '#424242',
      z: z
    )

    @resources.each_with_index do |count, index|
      next if count.zero?

      color = RESOURCE_COLORS[index]
      size = (tile_size / 6.0).clamp(3, 10)
      x_offset = draw_x + index % 3 * size
      y_offset = draw_y + index / 3 * size

      Square.new(
        x: x_offset, y: y_offset,
        size: size,
        color: color,
        z: z + 1
      )

      if count > 1
        Text.new(
          count.to_s,
          x: x_offset,
          y: y_offset,
          size: (size * 0.8),
          color: 'white',
          z: z + 2
        )
      end
    end

  end

  def x
    @position.x
  end

  def y
    @position.y
  end

  def to_s
    "Tile [#{@position.x}, #{@position.y}] -> Resources: #{@resources.join(', ')}"
  end
end
