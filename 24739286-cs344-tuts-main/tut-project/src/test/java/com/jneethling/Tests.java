package com.jneethling;

import java.util.List;
import java.util.HashSet;
import java.util.Set;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * The Tests class contains JUnit tests for various methods in the Master,
 * Module, and Venue classes.
 */
public class Tests {

    /**
     * Tests the generation of modules.
     */
    @Test
    public void testGenerateModules() {
        List<Module> modules = Master.generateModules(8);
        assertNotNull(modules);
        assertEquals(8, modules.size());
    }

    /**
     * Tests the generation of venues.
     */
    @Test
    public void testGenerateVenues() {
        List<Venue> venues = Master.generateVenues(9);
        assertNotNull(venues);
        assertEquals(9, venues.size());
    }

    /**
     * Tests the availability arrays of modules and venues.
     */
    @Test
    public void testAvailabilities() {
        List<Module> modules = Master.generateModules(8);
        List<Venue> venues = Master.generateVenues(9);
        int[] moduleAvailability = modules.get(0).getAvailability();
        int[] venueAvailability = venues.get(0).getAvailability();
        assertEquals(5, moduleAvailability.length);
        assertEquals(5, venueAvailability.length);
    }

    /**
     * Tests that module codes are unique.
     */
    @Test
    public void testNoRecurringModuleCodes() {
        List<Module> modules = Master.generateModules(8);
        Set<String> moduleCodes = new HashSet<>();

        for (Module module : modules) {
            boolean condition = !moduleCodes.contains(module.getModuleCode());
            assert condition : "Duplicate module code found: " + module.getModuleCode();
            moduleCodes.add(module.getModuleCode());
        }
    }

    /**
     * Tests that venue codes are unique.
     */
    @Test
    public void testNoRecurringVenueCodes() {
        List<Venue> venues = Master.generateVenues(9);
        Set<String> venueCodes = new HashSet<>();

        for (Venue venue : venues) {
            boolean condition = !venueCodes.contains(venue.getVenueCode());
            assert condition : "Duplicate venue code found: " + venue.getVenueCode();
            venueCodes.add(venue.getVenueCode());
        }
    }

}
