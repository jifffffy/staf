/*****************************************************************************/
/* Software Testing Automation Framework (STAF)                              */
/* (C) Copyright IBM Corp. 2009                                              */
/*                                                                           */
/* This software is licensed under the Eclipse Public License (EPL) V1.0.    */
/*****************************************************************************/

import com.zerog.ia.api.pub.*;
import java.io.*;
public class STAFCheckPythonVersion extends CustomCodeAction
{
    public void install(InstallerProxy ip)
    {
        PrintStream out = IASys.out;
        String usePythonSystemPath = ip.substitute("$USE_PYTHON_SYSTEM_PATH$");

        if (usePythonSystemPath.equals("1"))
        {
            // Try running "python -V" to determine the version of python
            // in the system path (if any)

            Process p = null;
            java.util.Properties envVars = new java.util.Properties();
            Runtime r = Runtime.getRuntime();
            String line = "";
            String pythonVersion = "";

            try
            {
                String command = "python -V";

                p = r.exec(command);

                // Note that "python -V" may output the version to either
                // standard error or standard output (depending on the Python
                // version).

                // First, check if the Python version is in stderr
                
                BufferedReader br = new BufferedReader(
                    new InputStreamReader(p.getErrorStream()));

                // The format of the output is usually "Python x.y.z" so
                // we remove the leading "Python " and instead of x.y.z
                // we only want x.y.  For example, the output may be
                // "Pythonn 2.7.0" and we'll want to extract "2.7" for
                // the version.
                
                while ((line = br.readLine()) != null)
                {
                    if (line.startsWith("Python "))
                    {
                        if (line.length() == 10)
                        {
                            // Format is "Python x.y" so extract x.y
                            pythonVersion = line.substring(7);
                        }
                        else
                        {
                            // Format is "Python x.y.z" so extract x.y.
                            String version = line.substring(7);
                            int lastPeriod = version.lastIndexOf('.');
                            pythonVersion = version.substring(0, lastPeriod);
                        }
                    }
                }
                
                br.close();
                
                if (pythonVersion == "")
                {
                    // Not in stderr, so now check for the version in stdout
                    
                    br = new BufferedReader(
                        new InputStreamReader(p.getInputStream()));
                
                    while ((line = br.readLine()) != null)
                    {
                        if (line.startsWith("Python "))
                        {
                            if (line.length() == 10)
                            {
                               // Format is "Python x.y" so extract x.y
                                pythonVersion = line.substring(7);
                            }
                            else
                            {
                                // Format is "Python x.y.z" so extract x.y
                                String version = line.substring(7);
                                int lastPeriod = version.lastIndexOf('.');
                                pythonVersion = version.substring(0, lastPeriod);
                            }
                        }
                    }
                
                    br.close();                   
                }    
            }
            catch (IOException e)
            {
                // The "python" command was not found in the system path.

                // Try running "python3 -V" because some Python V3.x installs
                // may only provide a python3 command, not a python command.

                try
                {
                    String command = "python3 -V";

                    p = r.exec(command);

                    // Note that "python3 -V" may output the version to either
                    // standard error or standard output (depending on the
                    // Python version).

                    // First, check if the Python version is in stdout                    

                    BufferedReader br = new BufferedReader(
                        new InputStreamReader(p.getInputStream()));

                    // The format of the output is usually "Python x.y.z" so
                    // we remove the leading "Python " and instead of x.y.z
                    // we only want x.y.  For example, the output may be
                    // "Pythonn 3.2.6" and we'll want to extract "3.2" for
                    // the version.
                
                    while ((line = br.readLine()) != null)
                    {
                        if (line.startsWith("Python "))
                        {
                            if (line.length() == 10)
                            {
                                // Format is "Python x.y" so extract x.y
                                pythonVersion = line.substring(7);
                            }
                            else
                            {
                                // Format is "Python x.y.z" so extract x.y
                                String version = line.substring(7);
                                int lastPeriod = version.lastIndexOf('.');
                                pythonVersion = version.substring(0, lastPeriod);
                            }
                        } 
                    }
                    
                    br.close();
                
                    if (pythonVersion == "")
                    {
                        // Not in stdout, so now check for the version in stderr
                    
                        br = new BufferedReader(
                            new InputStreamReader(p.getErrorStream()));
                
                        while ((line = br.readLine()) != null)
                        {
                            if (line.startsWith("Python "))
                            {
                                if (line.length() == 10)
                                {
                                    // Format is "Python x.y" so extract x.y
                                    pythonVersion = line.substring(7);
                                }
                                else
                                {
                                    // Format is "Python x.y.z" so extract x.y
                                    String version = line.substring(7);
                                    int lastPeriod = version.lastIndexOf('.');
                                    pythonVersion = version.substring(0, lastPeriod);
                                }
                            }
                        }
                
                        br.close();
                    }
                }
                catch (IOException ioe)
                {
                    // The "python3" command was not found in the system path.
                }
            }

            if (pythonVersion != "")
            {
                // Assign the Python version found in the system path to
                // variable USE_PYTHON_VERSION

                if (pythonVersion.equals("2.2"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.2");
                }
                else if (pythonVersion.equals("2.3"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.3");
                }
                else if (pythonVersion.equals("2.4"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.4");
                }
                else if (pythonVersion.equals("2.5"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.5");
                }
                else if (pythonVersion.equals("2.6"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.6");
                }
                else if (pythonVersion.equals("2.7"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "2.7");
                }
                else if (pythonVersion.equals("3.0"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "3.0");
                }
                else if (pythonVersion.equals("3.1"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "3.1");
                }
                else if (pythonVersion.equals("3.2"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "3.2");
                }
                else if (pythonVersion.equals("3.3"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "3.3");
                }
                else if (pythonVersion.equals("3.4"))
                {
                    ip.setVariable("$USE_PYTHON_VERSION$", "3.4");
                }
            }
        }
    }

    public void uninstall(UninstallerProxy up)
    {
    }

    public String getInstallStatusMessage()
    {
        return "";
    }

    public String getUninstallStatusMessage()
    {
        return "";
    }
}
