import random
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

SEAT_COUNT = 48
MESSAGE_TYPE_EVENT = 1
MESSAGE_NAME_EVENT = "event"
EVENT_CODE = 31
EVENT_NAME = "destination"
DESTINATIONS_FILE = "destinations.txt"
PRICE_PER_STOP = 1.63


@dataclass(frozen=True)
class Passenger:
    id: int
    pickup_point: int
    destination_point: int
    ticket_price: float


def load_destinations(file_path: str | Path) -> dict[int, str]:
    destinations: dict[int, str] = {}
    path = Path(file_path)

    if not path.exists():
        return destinations

    with path.open("r", encoding="utf-8") as file:
        for line_number, raw_line in enumerate(file, start=1):
            line = raw_line.strip()
            if not line or line.startswith("#"):
                continue

            try:
                destination_id_text, destination_name = line.split(maxsplit=1)
                destination_id = int(destination_id_text.strip())
                destinations[destination_id] = destination_name.strip()
            except ValueError:
                raise ValueError(
                    f"Invalid destination format on line {line_number}: {raw_line.rstrip()}"
                )

    return destinations


class Bus:
    def __init__(
        self,
        seat_count: int = SEAT_COUNT,
        destinations_file: str | Path | None = None,
    ) -> None:
        self.seat_count = seat_count
        self.passengers_on_board: list[Passenger] = []
        self.next_passenger_id = 1
        self.total_money_earned = 0.0

        base_dir = Path(__file__).resolve().parent
        destination_file_path = base_dir / (destinations_file or DESTINATIONS_FILE)
        self.destinations = load_destinations(destination_file_path)
        self.route_stops = list(self.destinations.keys())

    def get_destination_name(self, destination_code: int) -> str:
        return self.destinations.get(
            destination_code,
            f"Unknown destination ({destination_code})"
        )

    def _parse_destination_code(self, data: str) -> Optional[int]:
        try:
            parts = [part.strip() for part in data.split(";")]
            if len(parts) < 4:
                return None

            if parts[1].isdigit() and parts[2].isdigit():
                second_number = int(parts[1])
                third_number = int(parts[2])
                destination_code = int(parts[3])

                if second_number != MESSAGE_TYPE_EVENT or third_number != EVENT_CODE:
                    return None

            else:
                message_type = parts[1].lower()
                message_code = parts[2].lower()

                if message_type != MESSAGE_NAME_EVENT or message_code != EVENT_NAME:
                    return None

                value = parts[3]
                if ":" in value:
                    _, value_code = value.split(":", 1)
                    destination_code = int(value_code.strip())
                else:
                    destination_code = int(value)

            return destination_code

        except (ValueError, IndexError):
            return None

    def _get_passengers_getting_off(self, current_stop: int) -> list[Passenger]:
        passengers_getting_off = [
            passenger
            for passenger in self.passengers_on_board
            if passenger.destination_point == current_stop
        ]
        self.passengers_on_board = [
            passenger
            for passenger in self.passengers_on_board
            if passenger.destination_point != current_stop
        ]
        return passengers_getting_off

    def _generate_random_destination(self, pickup_point: int) -> Optional[int]:
        possible_destinations = [
            destination_code
            for destination_code in self.destinations
            if destination_code != pickup_point
        ]

        if not possible_destinations:
            return None

        return random.choice(possible_destinations)

    def _calculate_stop_distance(self, pickup_point: int, destination_point: int) -> int:
        pickup_index = self.route_stops.index(pickup_point)
        destination_index = self.route_stops.index(destination_point)

        if destination_index >= pickup_index:
            return destination_index - pickup_index

        return len(self.route_stops) - pickup_index + destination_index

    def _calculate_ticket_price(self, pickup_point: int, destination_point: int) -> float:
        stop_distance = self._calculate_stop_distance(pickup_point, destination_point)
        return round(PRICE_PER_STOP * stop_distance, 5)

    def _create_boarding_passengers(
        self,
        pickup_point: int,
        amount: int
    ) -> list[Passenger]:
        boarding_passengers: list[Passenger] = []

        for _ in range(amount):
            destination_point = self._generate_random_destination(pickup_point)
            if destination_point is None:
                break

            ticket_price = self._calculate_ticket_price(pickup_point, destination_point)

            passenger = Passenger(
                id=self.next_passenger_id,
                pickup_point=pickup_point,
                destination_point=destination_point,
                ticket_price=ticket_price,
            )

            self.next_passenger_id += 1
            boarding_passengers.append(passenger)

        return boarding_passengers

    def process_bus_arrival(self, data: str) -> Optional[dict]:
        destination_code = self._parse_destination_code(data)
        if destination_code is None:
            return None

        destination_name = self.get_destination_name(destination_code)

        passengers_getting_off = self._get_passengers_getting_off(destination_code)
        went_off = len(passengers_getting_off)

        free_seats = self.seat_count - len(self.passengers_on_board)
        requested_boarding = random.randint(0, free_seats)

        passengers_boarding = self._create_boarding_passengers(
            destination_code,
            requested_boarding
        )

        self.passengers_on_board.extend(passengers_boarding)
        came_on = len(passengers_boarding)

        money_earned_this_stop = round(
            sum(passenger.ticket_price for passenger in passengers_boarding),
            5
        )
        self.total_money_earned = round(
            self.total_money_earned + money_earned_this_stop,
            5
        )

        return {
            "destination_name": destination_name,
            "destination_code": destination_code,
            "went_off": went_off,
            "came_on": came_on,
            "on_board": len(self.passengers_on_board),
            "seat_count": self.seat_count,
            "money_earned_this_stop": money_earned_this_stop,
            "total_money_earned": self.total_money_earned,
            "passenger_destinations": [
                self.get_destination_name(passenger.destination_point)
                for passenger in self.passengers_on_board
            ],
        }


def format_bus_report(result: dict) -> str:
    return (
        f"Destination: {result['destination_name']}\n"
        f"People went off: {result['went_off']}\n"
        f"People came on: {result['came_on']}\n"
        f"People currently on the bus: {result['on_board']}/{result['seat_count']}\n"
        f"Money earned this stop: €{result['money_earned_this_stop']:.5f}\n"
        f"Total money earned: €{result['total_money_earned']:.5f}"
    )

