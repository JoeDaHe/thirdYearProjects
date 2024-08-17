package com.jneethling;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Map;
import java.util.HashMap;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.io.BufferedReader;
import java.io.FileReader;

/**
 * The Master class generates modules, venues, and a timetable, and writes them
 * to text files.
 */
public class Master {

        /**
         * The main method of the program.
         *
         * @param args The command-line arguments (NONE).
         */
        public static void main(String[] args) {

                // Generate data
                List<Module> modules = generateModules(8); // Generate 8 modules
                List<Venue> venues = generateVenues(9); // Generate 9 modules

                // Print the generated modules and venues
                writeModulesToFile(modules, "modules.txt");
                writeVenuesToFile(venues, "venues.txt");

                // Call GUI
                SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                                new GUI(modules, venues, "timetable.txt");
                        }
                });
        }

        /**
         * Generates a list of modules with random data.
         *
         * @param count The number of modules to generate.
         * @return A list of generated modules.
         */
        public static List<Module> generateModules(int count) {
                List<Module> modules = new ArrayList<>();
                Random random = new Random();

                for (int i = 0; i < count; i++) {
                        String moduleCode = "M" + i; // "M3"
                        String department = "Department " + (random.nextInt(3) + 1); // "Department3"
                        String lecturer = "Lecturer " + i; // "Lecturer 3"
                        int demand = random.nextInt(50) + 50; // "84"
                        int[] availability = generateAvailability(); // "[0110111001, 10101001111, 01110000101,
                                                                     // 01001110110, 01011000110]"

                        Module module = new Module(moduleCode, department, lecturer, demand, availability);
                        modules.add(module);
                }
                return modules;
        }

        /**
         * Generates a list of venues with random data.
         *
         * @param count The number of venues to generate.
         * @return A list of generated venues.
         */
        public static List<Venue> generateVenues(int count) {
                List<Venue> venues = new ArrayList<>();
                Random random = new Random();

                for (int i = 0; i < count; i++) {
                        String venueCode = "V" + i; // "V3"
                        String venueBuilding = "Building " + (random.nextInt(2) + 1); // "Building 3"
                        int capacity = random.nextInt(100) + 50; // "101"
                        int[] availability = generateAvailability(); // "[0110111001, 10101001111, 01110000101,
                                                                     // 01001110110, 01011000110]"

                        Venue venue = new Venue(venueCode, venueBuilding, capacity, availability);
                        venues.add(venue);
                }
                return venues;
        }

        /**
         * Generates a random availability array representing the availability per hour
         * per day.
         *
         * @return An array representing availability.
         */
        public static int[] generateAvailability() {
                Random random = new Random();
                int[] availability = new int[5]; // 11 hours from 08:00 to 18:00 over 5 days from Monday to Friday
                for (int i = 0; i < 5; i++) {
                        int binaryNumber = 0;
                        for (int j = 0; j < 11; j++) {
                                int bit = random.nextInt(2); // Generate a random bit (0 or 1)
                                binaryNumber = (binaryNumber << 1) | bit; // Shift left and OR with the bit
                        }
                        availability[i] = binaryNumber;
                }
                return availability;
        }

        /**
         * Writes a list of modules to a text file.
         *
         * @param modules  The list of modules to write.
         * @param fileName The name of the output file.
         */
        public static void writeModulesToFile(List<Module> modules, String fileName) {
                try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
                        for (Module module : modules) {
                                writer.write(module.toString());
                                writer.newLine();
                        }
                        System.out.println("Modules written to " + fileName);
                } catch (IOException e) {
                        e.printStackTrace();
                }
        }

        /**
         * Writes a list of venues to a text file.
         *
         * @param venues   The list of venues to write.
         * @param fileName The name of the output file.
         */
        public static void writeVenuesToFile(List<Venue> venues, String fileName) {
                try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
                        for (Venue venue : venues) {
                                writer.write(venue.toString());
                                writer.newLine();
                        }
                        System.out.println("Venues written to " + fileName);
                } catch (IOException e) {
                        e.printStackTrace();
                }
        }
}