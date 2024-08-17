package com.jneethling;

import java.util.ArrayList;
import java.util.List;

/**
 * The Module class represents a study module in an educational system.
 */
public class Module {
    private String moduleCode;
    private String department;
    private String lecturer;
    private int demand;
    private int[] availability; // An array to represent availability per hour per day (binary)
    // [monday, tuesday, 1011001111, ...]

    /**
     * Constructs a Module with the specified attributes.
     *
     * @param moduleCode   The unique code identifying the module.
     * @param department   The department to which the module belongs.
     * @param lecturer     The lecturer responsible for the module.
     * @param demand       The demand for the module.
     * @param availability An array representing availability per hour per day
     *                     (binary).
     */
    public Module(String moduleCode, String department, String lecturer, int demand, int[] availability) {
        this.moduleCode = moduleCode;
        this.department = department;
        this.lecturer = lecturer;
        this.demand = demand;
        this.availability = availability;
    }

    /**
     * Gets the module code.
     *
     * @return The module code.
     */
    public String getModuleCode() {
        return moduleCode;
    }

    /**
     * Sets the module code.
     *
     * @param moduleCode The module code to set.
     */
    public void setModuleCode(String moduleCode) {
        this.moduleCode = moduleCode;
    }

    /**
     * Gets the department to which the module belongs.
     *
     * @return The department.
     */
    public String getDepartment() {
        return department;
    }

    /**
     * Sets the department to which the module belongs.
     *
     * @param department The department to set.
     */
    public void setDepartment(String department) {
        this.department = department;
    }

    /**
     * Gets the lecturer responsible for the module.
     *
     * @return The lecturer.
     */
    public String getLecturer() {
        return lecturer;
    }

    /**
     * Sets the lecturer responsible for the module.
     *
     * @param lecturer The lecturer to set.
     */
    public void setLecturer(String lecturer) {
        this.lecturer = lecturer;
    }

    /**
     * Gets the demand for the module.
     *
     * @return The demand.
     */
    public int getDemand() {
        return demand;
    }

    /**
     * Sets the demand for the module.
     *
     * @param demand The demand to set.
     */
    public void setDemand(int demand) {
        this.demand = demand;
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
     * Returns a string representation of the Module object.
     *
     * @return A string representation containing module details.
     */
    @Override
    public String toString() {
        return "Module{" +
                "moduleCode='" + moduleCode + '\'' +
                ", department='" + department + '\'' +
                ", lecturer='" + lecturer + '\'' +
                ", demand=" + demand +
                ", availability=" + java.util.Arrays.toString(availability) +
                '}';
    }
}