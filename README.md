# Amusement Park Ride: Boarding-Passengers w/ Semaphores

<h2>Description</h2>
<p>In this project, I simulated an amusement park where passengers would enter the park and attempt to board a roller coaster ride. After riding the roller coaster some number of times, the passengers will leave the park. The program ends when all of the passengers exit the amusement park.</p>

<p>This program uses threads, where there is a single "car" thread, which serves as the ride in which passengers will board, as well as multiple "passenger" threads. The car thread has a maximum capacity, where there is a maximum number of passenger threads that can enter the car during the boarding process. During boarding, there needs to be a limit set on the number of passengers that are able to board the car at boarding time, which is when I use semaphores. Since semaphores use a counter to only let a certain number of passengers "board the car", this allows for passengers to ride with overflowing the car's capacity.</p>
