
'Copyright (C) 2006, 2007 Eric Ehlers

'This file is part of QuantLib, a free-software/open-source library
'for financial quantitative analysts and developers - http://quantlib.org/

'QuantLib is free software: you can redistribute it and/or modify it
'under the terms of the QuantLib license.  You should have received a
'copy of the license along with this program; if not, please email
'<quantlib-dev@lists.sf.net>. The license is also available online at
'<http://quantlib.org/license.shtml>.

'This program is distributed in the hope that it will be useful, but WITHOUT
'ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
'FOR A PARTICULAR PURPOSE.  See the license for more details.

' Module Upgrade - make any necessary changes to the user's environment
' following an upgrade of the QuantLibXL Launcher

Module Upgrade

    Private r_ As QuantLibXL.RegistryEditor
    Private registryVersion_ As Integer
    Public Const THIS_VERSION As Integer = 8

    Public Sub run()

        Try

            r_ = New QuantLibXL.RegistryEditor
            getVersionNumber()
            updateRegistry()
            r_ = Nothing
            Exit Sub

        Catch ex As Exception

            Dim msg As String = "QuantLibXL Launcher encountered an error " _
                & "while attempting to upgrade user preferences: " _
                & vbCrLf & vbCrLf & ex.Message & vbCrLf & vbCrLf _
                & "Do you want to restore default preferences?" & vbCrLf & vbCrLf _
                & "Click Yes to restore default preferences and continue processing." & vbCrLf & vbCrLf _
                & "Click No to exit the application."

            Dim res As MsgBoxResult = MsgBox(msg, MsgBoxStyle.YesNo + MsgBoxStyle.Critical, _
                "QuantLibXL Launcher Error")

            If res = MsgBoxResult.Yes Then

                Call restoreDefaults()

            Else

                Environment.Exit(1)

            End If

        End Try

    End Sub

    Private Sub restoreDefaults()

        Try

            r_ = New QuantLibXL.RegistryEditor
            r_.deleteKey("QuantLibXL Launcher")
            initializeRegistryLatestVersion()
            r_ = Nothing

            MsgBox("Default preferences successfully restored.")

        Catch ex As Exception

            MsgBox("QuantLibXL Launcher encountered an error " _
                & "while attempting to restore default preferences: " _
                & vbCrLf & vbCrLf & ex.Message & vbCrLf & vbCrLf _
                & "The application will now close, please contact support.")

            Environment.Exit(1)

        End Try

    End Sub

    Private Sub getVersionNumber()

        If r_.keyExists("QuantLibXL Launcher\LauncherVersion8") Then
            registryVersion_ = 8
        ElseIf r_.keyExists("QuantLibXL Launcher\LauncherVersion7") Then
            registryVersion_ = 7
        ElseIf r_.keyExists("QuantLibXL Launcher\LauncherVersion6") Then
            registryVersion_ = 6
        ElseIf r_.keyExists("QuantLibXL Launcher") Then
            registryVersion_ = 5
        ElseIf r_.keyExists("QuantLibXL") Then
            If r_.valueExists("QuantLibXL\Configuration", "Version") Then
                registryVersion_ = r_.getValue("QuantLibXL\Configuration", "Version")
            Else
                registryVersion_ = 1
            End If
        Else
            registryVersion_ = 0
        End If

    End Sub

    Private Sub updateRegistry()

        If registryVersion_ = 0 Then
            initializeRegistryLatestVersion()
            Exit Sub
        End If

        If registryVersion_ = 1 Then upgradeVersion1to2()
        If registryVersion_ = 2 Then upgradeVersion2to3()
        If registryVersion_ = 3 Then upgradeVersion3to4()
        If registryVersion_ = 4 Then upgradeVersion4to5()
        If registryVersion_ = 5 Then upgradeVersion5to6()
        If registryVersion_ = 6 Then upgradeVersion6to7()
        If registryVersion_ = 7 Then upgradeVersion7to8()

        ' Temporary hack - implement new feature without incrementing the launcher version number.
        ' If the version 8 registry key doesn't contain value ExcelPath then initialize it.
        ' This logic should be superceded by version 9 when it's implemented.
        If Not r_.valueExists("QuantLibXL Launcher\LauncherVersion8\Configuration", "ExcelPath") Then
            r_.setValue("QuantLibXL Launcher\LauncherVersion8\Configuration", _
                "ExcelPath", deriveDefaultExcelPath())
        End If

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''''''''''
    ' upgrade registry from previous launcher versions
    ''''''''''''''''''''''''''''''''''''''''''''''''''

    Private Sub upgradeVersion7to8()
        r_.copyKey("QuantLibXL Launcher\LauncherVersion7", "QuantLibXL Launcher\LauncherVersion8")
        Dim envKeyName As String
        Dim rootName As String
        rootName = "QuantLibXL Launcher\LauncherVersion8\Configuration\StartupActionsList"
        For Each keyName As String In r_.subKeyNames(rootName)
            envKeyName = rootName & "\" & keyName & "\CalibrateCMS"
            r_.createKey(envKeyName)
            r_.setValue(rootName & "\" & keyName, "CalibrateCMS", False)
        Next
        rootName = "QuantLibXL Launcher\LauncherVersion8\Environments"
        For Each keyName As String In r_.subKeyNames(rootName)
            envKeyName = rootName & "\" & keyName & "\StartupActions\" & "\CalibrateCMS"
            r_.createKey(envKeyName)
            r_.setValue(rootName & "\" & keyName & "\StartupActions", "CalibrateCMS", False)
        Next
        registryVersion_ = 8

    End Sub

    Private Sub upgradeVersion6to7()

        r_.copyKey("QuantLibXL Launcher\LauncherVersion6", "QuantLibXL Launcher\LauncherVersion7")
        r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "ReutersPath", _
            QuantLibXL.Configuration.REUTERS_PATH_DEFAULT & "\" & QuantLibXL.Configuration.REUTERS_XLA_DEFAULT)
        r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "BloombergPath", _
            QuantLibXL.Configuration.BLOOMBERG_PATH_DEFAULT & "\" & QuantLibXL.Configuration.BLOOMBERG_XLA_DEFAULT)
        r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "ReutersSelected", False)
        r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "BloombergSelected", False)

        registryVersion_ = 7

    End Sub

    Private Sub upgradeVersion5to6()

        r_.createKey("QuantLibXL Launcher")
        r_.copyKey("QuantLibXL", "QuantLibXL Launcher\LauncherVersion6")
        r_.deleteValue("QuantLibXL Launcher\LauncherVersion6\Configuration\", "Version")

        For Each environmentName As String In r_.subKeyNames("QuantLibXL Launcher\LauncherVersion6\Environments")
            Dim environmentKey As String = "QuantLibXL Launcher\LauncherVersion6\Environments\" & environmentName
            Dim addinDirectory As String = r_.getValue(environmentKey, "AddinDirectory")
            Dim addinName As String = r_.getValue(environmentKey, "AddinName")
            Dim addinFull As String
            If Len(addinDirectory) > 0 Then
                addinFull = addinDirectory & "\" & addinName
            Else
                addinFull = addinName
            End If
            r_.createKey(environmentKey & "\AddinList")
            r_.setValue(environmentKey & "\AddinList", "Addin0", addinFull)
            r_.deleteValue(environmentKey, "AddinDirectory")
            r_.deleteValue(environmentKey, "AddinName")

            r_.setValue(environmentKey, "FrameWorkVersion", 5)
        Next

        registryVersion_ = 6

    End Sub

    Private Sub upgradeVersion4to5()

        Dim envKeyName As String

        For Each keyName As String In r_.subKeyNames("QuantLibXL\Configuration\Environments")
            envKeyName = "QuantLibXL\Configuration\Environments\" & keyName & "\"
            r_.deleteValue(envKeyName, "AddinDirectory")
            r_.deleteValue(envKeyName, "AddinName")
            r_.deleteValue(envKeyName, "Framework")
            r_.deleteValue(envKeyName, "HelpFile")
            r_.deleteValue(envKeyName, "name")
            r_.deleteValue(envKeyName, "Workbooks")
        Next

        envKeyName = "QuantLibXL\Configuration\"
        r_.moveKey(envKeyName & "Environments", envKeyName & "StartupActionsList")

        For Each keyName As String In r_.subKeyNames("QuantLibXL\Environments")

            envKeyName = "QuantLibXL\Environments\" & keyName

            r_.setValue(envKeyName, "UserConfigurationFile", "C:\projects\Launcher\Users\users.xml")
            r_.setValue(envKeyName, "FunctionMetadata", "C:\projects\QuantLibAddin\gensrc\metadata")

            r_.createKey(envKeyName & "\StartupActions")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "CapVolBootstrap")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "FitCMS")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "IndexesTimeSeries")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "LoadBonds")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "LoadMurexYieldCurve")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "StaticData")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "SwapSmileBootstrap")
            'r_.moveValue(envKeyName, envKeyName & "\StartupActions", "SwapVolBootstrap")
            r_.moveValue(envKeyName, envKeyName & "\StartupActions", "YieldCurveBootstrap")

        Next

        r_.setValue("QuantLibXL\Configuration", "Version", 5)
        registryVersion_ = 5

    End Sub

    Private Sub upgradeVersion3to4()

        Dim envKeyName As String
        For Each keyName As String In r_.subKeyNames("QuantLibXL\Environments")
            envKeyName = "QuantLibXL\Environments\" & keyName
            r_.setValue(envKeyName, "StaticData", False)
        Next
        For Each keyName As String In r_.subKeyNames("QuantLibXL\Configuration\Environments")
            envKeyName = "QuantLibXL\Configuration\Environments\" & keyName
            r_.setValue(envKeyName, "StaticData", False)
        Next
        r_.setValue("QuantLibXL\Configuration", "Version", 4)
        registryVersion_ = 4

    End Sub

    Private Sub upgradeVersion2to3()

        Dim envKeyName As String
        For Each keyName As String In r_.subKeyNames("QuantLibXL\Environments")
            envKeyName = "QuantLibXL\Environments\" & keyName
            r_.setValue(envKeyName, "HelpFile", "")
            r_.setValue(envKeyName, "SwapSmileBootstrap", False)
            r_.setValue(envKeyName, "FitCMS", False)
            r_.setValue(envKeyName, "LoadBonds", False)
        Next
        r_.createKey("QuantLibXL\Configuration\Environments")
        r_.setValue("QuantLibXL\Configuration", "Version", 3)
        registryVersion_ = 3

    End Sub

    Private Sub upgradeVersion1to2()

        r_.moveKey("QuantLibXL\Configuration\Environments", "QuantLibXL\Environments")
        r_.deleteKey("QuantLibXL\Configuration\Users")
        r_.deleteKey("QuantLibXL\Launch")
        Dim envKeyName As String
        For Each keyName As String In r_.subKeyNames("QuantLibXL\Environments")
            envKeyName = "QuantLibXL\Environments\" & keyName
            r_.renameValue(envKeyName, "Bootstrap", "YieldCurveBootstrap")
            r_.setValue(envKeyName, "LoadMurexYieldCurve", False)
            r_.setValue(envKeyName, "CapVolBootstrap", False)
            'r_.setValue(envKeyName, "SwapVolBootstrap", False)
            r_.setValue(envKeyName, "IndexesTimeSeries", False)
        Next
        r_.setValue("QuantLibXL\Configuration", "Version", 2)
        registryVersion_ = 2

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' initialize registry for first use
    ''''''''''''''''''''''''''''''''''''''''''
    Private Sub initializeRegistryLatestVersion()

        Dim path As String
        path = "QuantLibXL Launcher\LauncherVersion" & THIS_VERSION & "\Configuration"
        r_.createKey(path)
        r_.setValue(path, "SelectedEnvConfig", "")
        r_.setValue(path, "SelectedEnvName", "")
        r_.setValue(path, "ReutersPath", QuantLibXL.Configuration.REUTERS_PATH_DEFAULT & "\" & QuantLibXL.Configuration.REUTERS_XLA_DEFAULT)
        r_.setValue(path, "BloombergPath", QuantLibXL.Configuration.BLOOMBERG_PATH_DEFAULT & "\" & QuantLibXL.Configuration.BLOOMBERG_XLA_DEFAULT)
        r_.setValue(path, "ReutersSelected", False)
        r_.setValue(path, "BloombergSelected", False)
        r_.setValue(path, "ExcelPath", deriveDefaultExcelPath())
        r_.createKey(path & "\StartupActionsList")
        r_.createKey(path & "\Environments")

    End Sub

    'Private Sub initializeRegistryVersion7()

    '    r_.createKey("QuantLibXL Launcher\LauncherVersion7\Configuration")
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "SelectedEnvName", "")
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "ReutersPath", _
    '        QuantLibXL.Configuration.REUTERS_PATH_DEFAULT)
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "BloombergPath", _
    '        QuantLibXL.Configuration.BLOOMBERG_PATH_DEFAULT)
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "ReutersSelected", False)
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion7\Configuration", "BloombergSelected", False)
    '    r_.createKey("QuantLibXL Launcher\LauncherVersion7\Configuration\StartupActionsList")
    '    r_.createKey("QuantLibXL Launcher\LauncherVersion7\Environments")

    'End Sub


    'Private Sub initializeRegistryVersion6()

    '    r_.createKey("QuantLibXL Launcher\LauncherVersion6\Configuration")
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion6\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL Launcher\LauncherVersion6\Configuration", "SelectedEnvName", "")
    '    r_.createKey("QuantLibXL Launcher\LauncherVersion6\Configuration\StartupActionsList")
    '    r_.createKey("QuantLibXL Launcher\LauncherVersion6\Environments")

    'End Sub

    'Private Sub initializeRegistryVersion5()

    '    r_.createKey("QuantLibXL\Configuration")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvName", "")
    '    r_.setValue("QuantLibXL\Configuration", "Version", 5)
    '    r_.createKey("QuantLibXL\Configuration\StartupActionsList")
    '    r_.createKey("QuantLibXL\Environments")

    'End Sub

    'Private Sub initializeRegistryVersion4()
    '
    '    r_.createKey("QuantLibXL\Configuration")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvName", "")
    '    r_.setValue("QuantLibXL\Configuration", "Version", 4)
    '    r_.createKey("QuantLibXL\Configuration\Environments")
    '    r_.createKey("QuantLibXL\Environments")
    '
    'End Sub

    'Private Sub initializeRegistryVersion3()
    '
    '    r_.createKey("QuantLibXL\Configuration")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvName", "")
    '    r_.setValue("QuantLibXL\Configuration", "Version", 3)
    '    r_.createKey("QuantLibXL\Configuration\Environments")
    '    r_.createKey("QuantLibXL\Environments")
    '
    'End Sub

    'Private Sub initializeRegistryVersion2()
    '
    '    r_.createKey("QuantLibXL\Configuration")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvConfig", "")
    '    r_.setValue("QuantLibXL\Configuration", "SelectedEnvName", "")
    '    r_.setValue("QuantLibXL\Configuration", "Version", 2)
    '    r_.createKey("QuantLibXL\Environments")
    '
    'End Sub

End Module
