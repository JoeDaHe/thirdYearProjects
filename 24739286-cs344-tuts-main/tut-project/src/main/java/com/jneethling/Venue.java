package com.jneethling;

import java.util.ArrayList;
import java.util.List;

/**
 * The Venue class represents a location where classes or events can be held.
 */
public class Venue {
    private String venueCode;
    private String venueBuilding;
    private int capacity;
    private int[] availability; // An array to represent availability per hour per day (binary)
    // [monday, tuesday, 1011001111, ...]

    /**
     * Constructs a Venue with the specified attributes.
     *
     * @param venueCode     The unique code identifying the venue.
     * @param venueBuilding The building or location where the venue is situated.
     * @param capacity      The maximum capacity of the venue.
     * @param availability  An array representing availability per hour per day
     *                      (binary).
     */
    public Venue(String venueCode, String venueBuilding, int capacity, int[] availability) {
        this.venueCode = venueCode;
        this.venueBuilding = venueBuilding;
        this.capacity = capacity;
        this.availability = availability;
    }

    /**
     * Gets the venue code.
     *
     * @return The venue code.
     */
    public String getVenueCode() {
        return venueCode;
    }

    /**
     * Sets the venue code.
     *
     * @param venueCode The venue code to set.
     */
    public void setVenueCode(String venueCode) {
        this.venueCode = venueCode;
    }

    /**
     * Gets the building where the venue is situated.
     *
     * @return The venue building.
     */
    public String getVenueBuilding() {
        return venueBuilding;
    }

    /**
     * Sets the building or location where the venue is situated.
     *
     * @param venueBuilding The venue building or location to set.
     */
    public void setVenueBuilding(String venueBuilding) {
        this.venueBuilding = venueBuilding;
    }

    /**
     * Gets the maximum capacity of the venue.
     *
     * @return The venue capacity.
     */
    public int getCapacity() {
        return capacity;
    }

    /**
     * Sets the maximum capacity of the venue.
     *
     * @param capacity The venue capacity to set.
     */
    public void setCapacity(int capacity) {
        this.capacity = capacity;
    }

    /**
     * Gets the availability array representing availability per hour per day
     * (binary).
     *
     * @return The availability array.
     */
    public int[] getAvailability() {
        return availability;
    }

    /**
     * Sets the availability array representing availability per hour per day
     * (binary).
     *
     * @param availability The availability array to set.
     */
    public void setAvailability(int[] availability) {
        this.availability = availability;
    }

    /**
     * Returns a string representation of the Venue object.
     *
     * @return A string representation containing venue details.
     */
    @Override
    public String toString() {
        return "Venue{" +
                "venueCode='" + venueCode + '\'' +
                ", venueBuilding='" + venueBuilding + '\'' +
                ", capacity=" + capacity +
                ", availability=" + java.util.Arrays.toString(availability) +
                '}';
    }
}
