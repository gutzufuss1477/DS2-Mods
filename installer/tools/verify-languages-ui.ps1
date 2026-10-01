param([Parameter(Mandatory=$true)][string]$ExePath, [Parameter(Mandatory=$true)][string]$OutputDirectory)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName PresentationFramework,PresentationCore,WindowsBase
$assembly = [System.Reflection.Assembly]::LoadFrom($ExePath)
$staticFlags = [System.Reflection.BindingFlags]'Public,NonPublic,Static'
$instanceFlags = [System.Reflection.BindingFlags]'Public,NonPublic,Instance'
$paths = $assembly.GetType('DS2ModSuite.AppPaths')
$slot = $paths.GetField('SelfTestDataSlot', $staticFlags).GetRawConstantValue()
$isolatedData = Join-Path $OutputDirectory 'isolated-ui-profile'
[AppDomain]::CurrentDomain.SetData($slot, $isolatedData)
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$app = New-Object System.Windows.Application
$app.ShutdownMode = [System.Windows.ShutdownMode]::OnExplicitShutdown
$theme = $assembly.GetType('DS2ModSuite.Theme').GetMethod('CreateResources', $staticFlags).Invoke($null, @())
$app.Resources.MergedDictionaries.Add($theme)
$catalog = $assembly.GetType('DS2ModSuite.CatalogService').GetMethod('LoadAndValidate', $staticFlags).Invoke($null, @())
$service = $assembly.GetType('DS2ModSuite.ModConfigurationService')
$profile = $service.GetMethod('LoadEffectiveProfile', $staticFlags).Invoke($null, @($catalog, $null))
$windowType = $assembly.GetType('DS2ModSuite.ModSettingsWindow')
$localization = $assembly.GetType('DS2ModSuite.Localization')
$setLanguage = $localization.GetMethod('SetLanguage', [type[]]@([string]))
function Save-Offscreen($root, [int]$width, [int]$height, [string]$name) {
    $root.Measure([System.Windows.Size]::new($width,$height))
    $root.Arrange([System.Windows.Rect]::new(0,0,$width,$height))
    $root.UpdateLayout()
    $bitmap = [System.Windows.Media.Imaging.RenderTargetBitmap]::new($width,$height,96,96,[System.Windows.Media.PixelFormats]::Pbgra32)
    $bitmap.Render($root)
    $encoder = [System.Windows.Media.Imaging.PngBitmapEncoder]::new()
    $encoder.Frames.Add([System.Windows.Media.Imaging.BitmapFrame]::Create($bitmap))
    $stream = [System.IO.File]::Create((Join-Path $OutputDirectory $name))
    try { $encoder.Save($stream) } finally { $stream.Dispose() }
}
foreach ($language in @('en','de','fr','es','it','pt-BR','ru','zh-CN','ja','ko')) {
    $null = $setLanguage.Invoke($null, @($language))
    $mainType = $assembly.GetType('DS2ModSuite.MainWindow')
    $main = [Activator]::CreateInstance($mainType, $instanceFlags, $null, [object[]]@($catalog), $null)
    $languages = $mainType.GetField('languageSelector', $instanceFlags).GetValue($main)
    if ($languages.Items.Count -ne 10 -or $languages.SelectedItem.Tag -ne $language) { throw 'Language selector mismatch.' }
    $states = $mainType.GetField('modStates', $instanceFlags).GetValue($main)
    foreach ($mod in $catalog.Mods) {
        $state = [Activator]::CreateInstance($assembly.GetType('DS2ModSuite.ModRuntimeState'), $true)
        $state.Spec = $mod
        $null = $assembly.GetType('DS2ModSuite.GameInspector').GetMethod('UpdateStatus', $staticFlags).Invoke($null, @($state))
        $states.Add($state)
    }
    $null = $mainType.GetMethod('BuildModList', $instanceFlags).Invoke($main, @())
    Save-Offscreen $main.Content 860 640 "main-$language.png"
    $main.Close()
    $window = [Activator]::CreateInstance($windowType, $instanceFlags, $null,
        [object[]]@($catalog, $profile, $null, [string[]]@('climbing-power-gloves-range','crafting-unlocks','sneaky-sam')), $null)
    $selector = $windowType.GetField('modSelector', $instanceFlags).GetValue($window)
    if ($selector.Items.Count -ne 2) { throw 'Installed configurable-mod filter failed in actual WPF window.' }
    foreach ($modId in @('climbing-power-gloves-range','crafting-unlocks')) {
        $selector.SelectedItem = @($selector.Items | Where-Object { $_.Mod.Id -eq $modId })[0]
        $advanced = $windowType.GetField('showAdvanced', $instanceFlags).GetValue($window)
        $advanced.IsChecked = $true
        $editors = $windowType.GetField('editors', $instanceFlags).GetValue($window)
        if ($modId -eq 'crafting-unlocks') {
            [string]$choiceKey = @($editors.Keys | Where-Object { $_ -like '*|Items|*' })[0]
            $combo = $editors[$choiceKey]
            if ($combo.Items.Count -ne 3 -or $combo.Items[2].Tag -ne 'inherit') { throw 'Crafting choice values were not rendered.' }
            $combo.SelectedIndex = 2
            $null = $windowType.GetMethod('CaptureEditors', $instanceFlags).Invoke($window, @())
            $captured = $windowType.GetField('profile', $instanceFlags).GetValue($window)
            $value = $service.GetMethod('GetValue', $staticFlags).Invoke($null, @($captured, $choiceKey))
            if ($value -ne 'inherit') { throw 'Displayed choice did not round-trip to INI value.' }
        }
        Save-Offscreen $window.Content 760 600 "$modId-$language.png"
        if ($modId -eq 'crafting-unlocks') {
            $scroll = @($window.Content.Children | Where-Object { $_ -is [System.Windows.Controls.ScrollViewer] })[0]
            $scroll.ScrollToVerticalOffset(1300)
            Save-Offscreen $window.Content 760 600 "crafting-choices-$language.png"
        }
        Write-Output "PASS offscreen $modId $language ($($editors.Count) controls)"
    }
    $window.Close()
}
$app.Shutdown()
