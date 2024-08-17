package com.jneethling;

import javax.print.DocFlavor;
import javax.sound.sampled.Line;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * This class represents a graphical user interface for generating timetables.
 */
public class GUI extends JFrame {
    private JButton generateButton;
    private JTextArea previewTextArea;

    /**
     * Constructor for the GUI class.
     *
     * @param modules  The list of modules.
     * @param venues   The list of venues.
     * @param fileName The name of the output file.
     */
    public GUI(List<Module> modules, List<Venue> venues, String fileName) {
        super("Timetable Generator");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(700, 600);

        JPanel panel = new JPanel();
        getContentPane().add(panel);

        generateButton = new JButton("Generate Timetable");
        panel.add(generateButton);

        // Create a JTextArea for the timetable preview
        previewTextArea = new JTextArea(30, 60);
        previewTextArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(previewTextArea);
        panel.add(scrollPane);

        generateButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                generateTimetable(modules, venues, fileName, previewTextArea);
            }
        });
        setVisible(true);
    }

    /**
     * Generates a timetable based on module and venue availability and writes it to
     * a text file.
     *
     * @param modules  The list of modules.
     * @param venues   The list of venues.
     * @param fileName The name of the output file.
     */
    public static void generateTimetable(List<Module> modules, List<Venue> venues, String fileName,
            JTextArea textArea) {

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
            Random random = new Random();

            // Define the days of the week and hours of the day
            String[] daysOfWeek = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" };
            String[] hoursOfDay = { "08:00 AM", "09:00 AM", "10:00 AM", "11:00 AM", "12:00 PM",
                    "01:00 PM", "02:00 PM", "03:00 PM", "04:00 PM", "05:00 PM", "06:00 PM" };

            for (Module module : modules) {
                writer.write("Module: " + module.getModuleCode());
                writer.newLine();

                for (String day : daysOfWeek) {
                    writer.write(day + ": ");

                    // Generate a random venue index for this day
                    int venueIndex = random.nextInt(venues.size());
                    Venue venue = venues.get(venueIndex);

                    // Check if the venue is available during this day
                    int[] venueAvailability = venue.getAvailability();
                    int[] moduleAvailability = module.getAvailability();

                    // Choose a random hour when both the module and venue are available
                    int hourIndex = -1;
                    do {
                        hourIndex = random.nextInt(hoursOfDay.length);
                    } while (hourIndex < venueAvailability.length
                            && hourIndex < moduleAvailability.length &&
                            venueAvailability[hourIndex] != 1
                            && moduleAvailability[hourIndex] != 1);

                    // Print the chosen venue and time slot
                    String chosenTimeSlot = hoursOfDay[hourIndex];
                    writer.write(chosenTimeSlot + " - " + venue.getVenueCode());
                    writer.newLine();
                }

                writer.newLine();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

        try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
            StringBuilder content = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                content.append(line).append("\n"); // Append each line with a newline character
            }
            String fileContent = content.toString();
            // Set the content to the JTextArea
            textArea.setText(fileContent);
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("Timetable written to timetable.txt");

    }
}