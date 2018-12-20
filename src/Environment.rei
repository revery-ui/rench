/* 
 * Environment
 * 
 * Module for working with the OS / Environment variables
 * Similiar to the 'os'/process modules in Node
 */

let which: string => option(string);

let newLine: string;

let getEnvironmentVariables: unit => EnvironmentVariables.t;

let getEnvironmentVariable: string => option(string);

let getWorkingDirectory: unit => string;

let getExecutingDirectory: unit => string;
