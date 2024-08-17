# University Timetable Generator

## Overview

This Java project is a University Timetable Generator that allows you to generate timetables for modules and venues. 
It includes classes for modules and venues, random data generation, and a scheduling algorithm to assign modules to available venues.

## Table of Contents

1. Prerequisites
2. Project Structure
3. Usage
3.1 Generating Modules and Venues
3.2 Generating a Timetable
3.3 Running Tests
3.4 License

## Prerequisites

- Java JDK (Java Development Kit) installed on your system.
- An Integrated Development Environment (IDE) such as Eclipse or IntelliJ IDEA (optional).

## Project Structure

- `com.jneethling` package contains the main classes of the project:
  - `Master.java`: The main class for generating modules, venues, and timetables.
  - `Module.java`: Represents a study module.
  - `Venue.java`: Represents a location where classes or events can be held.
  - `GUI.java`: Represents a graphical user interface for generating timetables.

- `tests` package contains JUnit test cases for the project.
- `README.txt`: This README file.

## Usage

Follow the steps below to use the University Timetable Generator:

### Generating Modules and Venues

1. Open the `Master.java` file in your preferred Java IDE or text editor.

2. Customize the number of modules and venues to generate by modifying the values in the `main` method:
   ```java
   List<Module> modules = generateModules(8); // Generate 8 modules
   List<Venue> venues = generateVenues(9);   // Generate 9 venues

### Genrating a Timetable

1. To generate a timetable, run the project and interact with the GUI.
    There is a button for generating new timetables, the timetable will save on generation.

### Running Tests

1. Open the `Tests.java` file located in the `tests` package.

2. Run the JUnit test cases to verify the correctness of the project's functionality. 
    The tests cover various aspects of module and venue generation.

3. Pages house the test results:
    Jacoco tests
